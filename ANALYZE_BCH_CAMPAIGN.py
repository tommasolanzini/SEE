"""
ANALYZE_BCH_CAMPAIGN.py

Aggregates the EDAC campaign CSVs written by PC_INTERFACE.save_results, named

    Test_<DD_MM_YYYY>_0f_10k_0<N>.csv        with  N = 1 .. 30

(each row: word_index, bch_status, crc, payload_match, diff_bytes,
 sent_word_hex, corrected_word_hex)

What it computes
----------------
1.  FLIP DISTRIBUTION -- fraction of words that took 0, 1, 2, ... bit-flips.
    The CSV stores the *corrected* payload, not the raw codeword, so the
    injected-flip count cannot be recomputed from sent XOR recv (a corrected
    word is identical to the sent one by definition). The flip count is taken
    from `bch_status`: the decoder reports how many errors it corrected
    (0 = clean, 1, 2 = corrected, <0 = uncorrectable, i.e. >= 3 flips or a
    decoder failure -- bucketed as "FAIL").

2.  BCH SUCCESS RATE PER BUCKET -- within each flip bucket, the fraction of
    words whose payload came back bit-identical to what was sent
    (`payload_match == OK`). This is an INDEPENDENT check: bch_status saying
    "corrected 1" does not guarantee the payload is right (miscorrection),
    so success is judged on sent-vs-recv, not on the decoder's own claim.
    e.g. "14% of words got 0 flips, and with 0 flips the BCH path delivered
    an intact payload 98% of the time".

3.  TEST-TO-TEST STATISTICS -- every per-test fraction / success rate is
    computed per file, then summarised across the 30 tests as mean +/- std
    (sample std, ddof=1). Pooled (all words together) values are also given.

Outputs (next to the inputs, in .\\Test_Output\\):
    * bch_campaign_per_test.csv   one row per test: fractions + success rates
    * bch_campaign_summary.csv    one row per bucket: mean/std/pooled
    * bch_campaign_plot.png       two panels: distribution + success rate
plus a console summary.
"""

import os
import re
import csv
import glob
import math
from collections import defaultdict

import numpy as np
import matplotlib.pyplot as plt

# --- Configuration ----------------------------------------------------------
OUTPUT_DIR = os.path.join(".", "Test_Output")
ID_PREFIX  = "0f_10k"      # campaign id in the filename: Test_<date>_0f_10k_0<N>.csv
N_MIN      = 1
N_MAX      = 30
FAIL       = "FAIL"        # bucket label for bch_status < 0 (uncorrectable)

PER_TEST_CSV = os.path.join(OUTPUT_DIR, "bch_campaign_per_test.csv")
SUMMARY_CSV  = os.path.join(OUTPUT_DIR, "bch_campaign_summary.csv")
PLOT_PNG     = os.path.join(OUTPUT_DIR, "bch_campaign_plot.png")

_POP = bytes(bin(i).count("1") for i in range(256))


def residual_bit_flips(sent_hex, recv_hex):
    """Bits still wrong AFTER correction (only meaningful on mismatched rows)."""
    try:
        a, b = bytes.fromhex(sent_hex), bytes.fromhex(recv_hex)
    except ValueError:
        return None
    if len(a) != len(b) or not a:
        return None
    return sum(_POP[x ^ y] for x, y in zip(a, b))


def collect_run_files():
    """Map run number N -> csv path. The leading '0' written by the interface
    (raw_id = f"0f_10k_0{N}") is absorbed by int(): '01' -> 1, '030' -> 30."""
    rx = re.compile(rf"_{re.escape(ID_PREFIX)}_(\d+)\.csv$", re.IGNORECASE)
    found = defaultdict(list)
    for path in sorted(glob.glob(os.path.join(OUTPUT_DIR, f"Test_*_{ID_PREFIX}_*.csv"))):
        m = rx.search(os.path.basename(path))
        if m:
            found[int(m.group(1))].append(path)

    runs = {}
    for n, paths in found.items():
        if len(paths) > 1:
            print(f"  WARNING: run {n} matches {len(paths)} files; "
                  f"using '{os.path.basename(paths[-1])}'")
        runs[n] = paths[-1]
    return runs


def analyze_run(path):
    """Per-test tallies: counts/matches per flip bucket + side stats."""
    counts   = defaultdict(int)   # bucket -> words in bucket
    matches  = defaultdict(int)   # bucket -> words with payload OK
    crc_ok_n = defaultdict(int)   # bucket -> words with CRC OK
    silent   = 0                  # decoder claimed success but payload wrong
    resid    = []                 # residual flipped bits on mismatched words
    total    = 0

    with open(path, newline="") as f:
        for r in csv.DictReader(f):
            try:
                bch = int(r["bch_status"])
            except (KeyError, ValueError, TypeError):
                continue
            total += 1

            bucket = FAIL if bch < 0 else bch
            ok     = (r.get("payload_match", "").strip().upper() == "OK")
            crc    = (r.get("crc", "").strip().upper() == "OK")

            counts[bucket] += 1
            if ok:
                matches[bucket] += 1
            if crc:
                crc_ok_n[bucket] += 1
            if not ok:
                if bch >= 0:
                    silent += 1
                nf = residual_bit_flips(r.get("sent_word_hex", ""),
                                        r.get("corrected_word_hex", ""))
                if nf is not None:
                    resid.append(nf)

    return {"total": total, "counts": counts, "matches": matches,
            "crc_ok": crc_ok_n, "silent": silent, "resid": resid}


def bucket_sort_key(b):
    return (1, 0) if b == FAIL else (0, b)


def fmt_pct(x):
    return "   --  " if (x is None or (isinstance(x, float) and math.isnan(x))) \
           else f"{x:7.2%}"


def main():
    runs = collect_run_files()
    if not runs:
        print(f"No campaign files found in '{OUTPUT_DIR}' "
              f"(expected Test_*_{ID_PREFIX}_0<N>.csv).")
        return

    print(f"Scanning '{OUTPUT_DIR}' for {ID_PREFIX} runs {N_MIN}..{N_MAX} ...")
    per_run = {}
    for n in range(N_MIN, N_MAX + 1):
        path = runs.get(n)
        if path is None:
            print(f"  run {n:<3}: MISSING (skipped)")
            continue
        per_run[n] = analyze_run(path)
        print(f"  run {n:<3}: {per_run[n]['total']:>6} words   "
              f"({os.path.basename(path)})")

    if not per_run:
        print("\nNo usable runs -- nothing to aggregate.")
        return

    # Every bucket seen anywhere, in order 0,1,2,...,FAIL
    buckets = sorted({b for s in per_run.values() for b in s["counts"]},
                     key=bucket_sort_key)

    # --- Per-test fractions and success rates (NaN when bucket absent) ------
    run_ids = sorted(per_run)
    frac = {b: [] for b in buckets}   # fraction of words in bucket, per test
    succ = {b: [] for b in buckets}   # payload-OK rate within bucket, per test
    for n in run_ids:
        s = per_run[n]
        for b in buckets:
            c = s["counts"].get(b, 0)
            frac[b].append(c / s["total"] if s["total"] else np.nan)
            succ[b].append(s["matches"].get(b, 0) / c if c else np.nan)

    # --- Pooled (all words of all tests together) ----------------------------
    pool_total   = sum(s["total"] for s in per_run.values())
    pool_counts  = {b: sum(s["counts"].get(b, 0)  for s in per_run.values()) for b in buckets}
    pool_matches = {b: sum(s["matches"].get(b, 0) for s in per_run.values()) for b in buckets}
    pool_crc     = {b: sum(s["crc_ok"].get(b, 0)  for s in per_run.values()) for b in buckets}
    pool_silent  = sum(s["silent"] for s in per_run.values())
    pool_resid   = [x for s in per_run.values() for x in s["resid"]]

    # --- Mean / std across the test set (sample std, ddof=1) ----------------
    def mstd(values):
        v = np.asarray(values, dtype=float)
        v = v[~np.isnan(v)]
        if v.size == 0:
            return np.nan, np.nan
        return float(np.mean(v)), float(np.std(v, ddof=1)) if v.size > 1 else 0.0

    stats = {}
    for b in buckets:
        fm, fs = mstd(frac[b])
        sm, ss = mstd(succ[b])
        stats[b] = {"frac_mean": fm, "frac_std": fs,
                    "succ_mean": sm, "succ_std": ss}

    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # --- bch_campaign_per_test.csv ------------------------------------------
    with open(PER_TEST_CSV, "w", newline="") as f:
        w = csv.writer(f)
        hdr = ["run", "words"]
        for b in buckets:
            hdr += [f"n_{b}", f"frac_{b}", f"bch_success_{b}"]
        hdr += ["silent_corruptions"]
        w.writerow(hdr)
        for i, n in enumerate(run_ids):
            s   = per_run[n]
            row = [n, s["total"]]
            for b in buckets:
                c  = s["counts"].get(b, 0)
                fr = frac[b][i]
                sr = succ[b][i]
                row += [c,
                        f"{fr:.8f}" if not math.isnan(fr) else "",
                        f"{sr:.8f}" if not (isinstance(sr, float) and math.isnan(sr)) else ""]
            row.append(s["silent"])
            w.writerow(row)

    # --- bch_campaign_summary.csv --------------------------------------------
    with open(SUMMARY_CSV, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["bucket", "pooled_words", "pooled_fraction",
                    "frac_mean", "frac_std",
                    "pooled_bch_success", "succ_mean", "succ_std",
                    "pooled_crc_ok_rate"])
        for b in buckets:
            c  = pool_counts[b]
            st = stats[b]
            w.writerow([b, c,
                        f"{c / pool_total:.8f}",
                        f"{st['frac_mean']:.8f}", f"{st['frac_std']:.8f}",
                        f"{pool_matches[b] / c:.8f}" if c else "",
                        f"{st['succ_mean']:.8f}" if not math.isnan(st["succ_mean"]) else "",
                        f"{st['succ_std']:.8f}"  if not math.isnan(st["succ_std"])  else "",
                        f"{pool_crc[b] / c:.8f}" if c else ""])

    # --- Console summary ------------------------------------------------------
    print(f"\nWrote per-test table -> {PER_TEST_CSV}")
    print(f"Wrote summary table  -> {SUMMARY_CSV}")

    print("\n" + "=" * 74)
    print(f"  BCH CAMPAIGN  --  {len(run_ids)} tests, {pool_total} words total")
    print("=" * 74)
    print(f"{'flips':>8} | {'words':>8} | {'fraction (mean+/-std)':>24} | "
          f"{'BCH success (mean+/-std)':>26}")
    print("-" * 74)
    for b in buckets:
        st = stats[b]
        lbl = str(b) if b != FAIL else ">=3/FAIL"
        f_str = f"{fmt_pct(st['frac_mean'])} +/- {st['frac_std']:6.2%}"
        s_str = (f"{fmt_pct(st['succ_mean'])} +/- {st['succ_std']:6.2%}"
                 if not math.isnan(st["succ_mean"]) else "      --")
        print(f"{lbl:>8} | {pool_counts[b]:>8} | {f_str:>24} | {s_str:>26}")
    print("-" * 74)
    print(f"Silent corruptions (BCH claimed OK, payload wrong): {pool_silent}")
    if pool_resid:
        print(f"Residual flipped bits on mismatched words: "
              f"mean {np.mean(pool_resid):.1f}, max {max(pool_resid)}")
    print("=" * 74)

    # --- Plot -----------------------------------------------------------------
    x      = np.arange(len(buckets))
    labels = [str(b) if b != FAIL else ">=3\n(FAIL)" for b in buckets]
    fmeans = [stats[b]["frac_mean"] for b in buckets]
    fstds  = [stats[b]["frac_std"]  for b in buckets]
    smeans = [stats[b]["succ_mean"] for b in buckets]
    sstds  = [stats[b]["succ_std"]  for b in buckets]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4.5))
    fig.suptitle(f"EDAC campaign '{ID_PREFIX}' -- {len(run_ids)} tests, "
                 f"{pool_total} words (mean ± std across tests)", fontsize=12)

    ax1.bar(x, fmeans, yerr=fstds, capsize=4, color="#1A5F9E", alpha=0.85)
    ax1.set_xticks(x); ax1.set_xticklabels(labels)
    ax1.set_xlabel("Bit-flips per word (from BCH decoder)")
    ax1.set_ylabel("Fraction of words [-]")
    ax1.set_title("Flip distribution")
    ax1.grid(True, axis="y", linestyle="--", linewidth=0.5, alpha=0.6)
    for xi, m in zip(x, fmeans):
        if not math.isnan(m):
            ax1.annotate(f"{m:.2%}", (xi, m), ha="center", va="bottom",
                         fontsize=8, xytext=(0, 3), textcoords="offset points")

    ok = [i for i, m in enumerate(smeans) if not math.isnan(m)]
    ax2.bar([x[i] for i in ok], [smeans[i] for i in ok],
            yerr=[sstds[i] for i in ok], capsize=4, color="#2E8B57", alpha=0.85)
    ax2.set_xticks(x); ax2.set_xticklabels(labels)
    ax2.set_ylim(0, 1.05)
    ax2.set_xlabel("Bit-flips per word (from BCH decoder)")
    ax2.set_ylabel("Payload intact after decode [-]")
    ax2.set_title("BCH success rate per bucket")
    ax2.grid(True, axis="y", linestyle="--", linewidth=0.5, alpha=0.6)
    for i in ok:
        ax2.annotate(f"{smeans[i]:.2%}", (x[i], smeans[i]), ha="center",
                     va="bottom", fontsize=8, xytext=(0, 3),
                     textcoords="offset points")

    fig.tight_layout()
    fig.savefig(PLOT_PNG, dpi=150)
    print(f"Saved plot -> {PLOT_PNG}")
    plt.show()


if __name__ == "__main__":
    main()
