"""
ANALYZE_DR_FLIPS.py

Aggregates the per-word results of the DryRun (DR) link-integrity campaign and
recovers the distribution of bit-flips seen on the PC-MCU-NAND path
(e.g. flips induced by radiation during a SEE run).

It scans  .\\Test_Output\\  for the CSV files written by
PC_INTERFACE.save_results, one per run, named

    Test_<DD_MM_YYYY>_DR_<N>.csv      with  N = 1 .. 15

For every word in every run it counts the EXACT number of flipped BITS as

    popcount( sent_word_hex  XOR  recv_word_hex )

The CSV's `diff_bytes` column only says how many *bytes* changed, not how many
*bits*, so we recompute from the stored hex to stay bit-accurate.

Why a 2-D array (word x run) and not a single per-word total?
    Keeping one number per (word, run) is what lets us recover the *distribution*
    afterwards. A per-word total of 3 over 15 runs could be one run with 3 flips
    or three runs with 1 flip each -- two very different distributions. The 2-D
    array preserves both; the totals are just a derived column.

Outputs (written next to the inputs, in .\\Test_Output\\):
    * flips_matrix_DR.csv         rows = word_index, cols = DR_1..DR_15,
                                  cell = #bit-flips ('' = absent, 'SHORT' = unusable)
    * flips_distribution_DR.csv   k, word_count, fraction
                                  -> how many word-observations had exactly k flips
and prints the distribution (0-flip / 1-flip / ...) to the console.
"""

import os
import re
import csv
import glob
from collections import defaultdict

# --- Configuration ----------------------------------------------------------
OUTPUT_DIR = os.path.join(".", "Test_Output")
ID_PREFIX  = "DR"          # test IDs are DR_1 .. DR_<N_MAX>
N_MIN      = 1
N_MAX      = 15
SHORT_FLAG = -1            # sentinel kept in the matrix for SHORT / unusable rows

MATRIX_CSV = os.path.join(OUTPUT_DIR, "flips_matrix_DR.csv")
DIST_CSV   = os.path.join(OUTPUT_DIR, "flips_distribution_DR.csv")

# Per-byte popcount lookup (version-independent, fast enough for offline use).
_POP = bytes(bin(i).count("1") for i in range(256))


def bit_flips(sent_hex, recv_hex):
    """Exact number of differing BITS between two hex strings.
    Returns None when the two cannot be compared (missing / unequal length)."""
    if not sent_hex or not recv_hex:
        return None
    try:
        a = bytes.fromhex(sent_hex)
        b = bytes.fromhex(recv_hex)
    except ValueError:
        return None
    if len(a) == 0 or len(a) != len(b):
        return None
    return sum(_POP[x ^ y] for x, y in zip(a, b))


def collect_run_files():
    """Map run number -> csv path for every  Test_*_DR_<n>.csv  in OUTPUT_DIR.

    The trailing  _DR_<n>.csv  is anchored with a regex so DR_1 does not also
    grab DR_10..DR_15, and so the (variable) date prefix is ignored."""
    rx = re.compile(rf"_{ID_PREFIX}_(\d+)\.csv$", re.IGNORECASE)
    found = defaultdict(list)
    pattern = os.path.join(OUTPUT_DIR, f"Test_*_{ID_PREFIX}_*.csv")
    for path in sorted(glob.glob(pattern)):
        m = rx.search(os.path.basename(path))
        if m:
            found[int(m.group(1))].append(path)

    runs = {}
    for n, paths in found.items():
        if len(paths) > 1:
            print(f"  WARNING: DR_{n} matches {len(paths)} files; "
                  f"using '{os.path.basename(paths[-1])}'")
        runs[n] = paths[-1]
    return runs


def main():
    runs = collect_run_files()
    if not runs:
        print(f"No DR campaign files found in '{OUTPUT_DIR}' "
              f"(expected Test_*_{ID_PREFIX}_<N>.csv).")
        return

    # flips_by_word[word_index] -> { run_n : bit_flip_count | SHORT_FLAG }
    flips_by_word = defaultdict(dict)
    # distribution[k] -> number of word-observations with exactly k bit-flips
    distribution = defaultdict(int)
    short_rows = 0
    total_obs  = 0
    max_word   = 0
    present_runs = []

    print(f"Scanning '{OUTPUT_DIR}' for DR_{N_MIN}..DR_{N_MAX} ...")
    for n in range(N_MIN, N_MAX + 1):
        path = runs.get(n)
        if path is None:
            print(f"  DR_{n:<2}: MISSING (skipped)")
            continue
        present_runs.append(n)

        rows_seen = 0
        with open(path, newline="") as f:
            reader = csv.DictReader(f)
            for r in reader:
                rows_seen += 1
                try:
                    widx = int(r["word_index"])
                except (KeyError, ValueError, TypeError):
                    continue
                max_word = max(max_word, widx)

                result = (r.get("result") or "").strip().upper()
                nf = bit_flips(r.get("sent_word_hex", ""), r.get("recv_word_hex", ""))

                if result == "SHORT" or nf is None:
                    flips_by_word[widx][n] = SHORT_FLAG
                    short_rows += 1
                    continue

                flips_by_word[widx][n] = nf
                distribution[nf] += 1
                total_obs += 1

        print(f"  DR_{n:<2}: {rows_seen:>6} words   ({os.path.basename(path)})")

    if total_obs == 0:
        print("\nNo usable word-observations found -- nothing to aggregate.")
        return

    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # --- Write the per-word flip matrix (the 'array' requested) --------------
    run_cols = list(range(N_MIN, N_MAX + 1))
    with open(MATRIX_CSV, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["word_index"] + [f"{ID_PREFIX}_{n}" for n in run_cols]
                   + ["total_flips", "runs_hit"])
        for widx in range(1, max_word + 1):
            per_run = flips_by_word.get(widx, {})
            cells, total, hits = [], 0, 0
            for n in run_cols:
                v = per_run.get(n)
                if v is None:
                    cells.append("")            # run absent or word not present
                elif v == SHORT_FLAG:
                    cells.append("SHORT")
                else:
                    cells.append(v)
                    total += v
                    if v > 0:
                        hits += 1
            w.writerow([widx] + cells + [total, hits])

    # --- Write the flip distribution ----------------------------------------
    keys = sorted(set(distribution) | {0})       # always show the 0-flip row
    with open(DIST_CSV, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["bit_flips", "word_count", "fraction"])
        for k in keys:
            c = distribution.get(k, 0)
            w.writerow([k, c, f"{c / total_obs:.8f}"])

    # --- Console summary ----------------------------------------------------
    untouched   = distribution.get(0, 0)
    single      = distribution.get(1, 0)
    multi       = total_obs - untouched - single
    total_flips = sum(k * c for k, c in distribution.items())

    print(f"\nWrote per-word flip matrix -> {MATRIX_CSV}")
    print(f"Wrote flip distribution    -> {DIST_CSV}")

    print("\n" + "=" * 50)
    print("  DR CAMPAIGN -- BIT-FLIP DISTRIBUTION")
    print("=" * 50)
    print(f"Runs analysed:        {present_runs}")
    print(f"Word-observations:    {total_obs}")
    print(f"SHORT/unusable rows:  {short_rows}")
    print(f"Total bit-flips:      {total_flips}")
    print(f"Untouched (0 flips):  {untouched}  ({untouched/total_obs:.2%})")
    print(f"Single flip (1):      {single}  ({single/total_obs:.2%})")
    print(f"Multi  flip (>=2):    {multi}  ({multi/total_obs:.2%})")
    print("-" * 50)
    print(f"{'bit-flips':>10} | {'words':>8} | fraction")
    print("-" * 50)
    for k in keys:
        c = distribution.get(k, 0)
        frac = c / total_obs
        bar = "#" * int(round(frac * 30))
        print(f"{k:>10} | {c:>8} | {frac:7.2%} {bar}")
    print("=" * 50)


if __name__ == "__main__":
    main()
