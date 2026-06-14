# Outputs (in .\\Test_Output_MCU\\), per campaign n:
#     * bch_mcu_<n>f_per_test.csv    one row per test
#     * bch_mcu_<n>f_summary.csv     one row per bucket
#     * bch_mcu_<n>f_plot.png        two panels: distribution + success rate
# plus a cross-campaign overview:
#     * bch_mcu_overview.png         payload-intact rate & decoder verdicts
#                                    as a function of the injected flips

import os
import re
import csv
import glob
import math
from collections import defaultdict

import numpy as np
import matplotlib.pyplot as plt

OUTPUT_DIR     = os.path.join(".", "Software_Test", "Test_Output_MCU")
INJECTED_FLIPS = [0, 1, 2, 3]          # campaigns to analyse
ID_TEMPLATE    = "{n}f_10k_MCU"        # filename id: Test_<date>_<id>_<N>.csv
N_MIN          = 1
N_MAX          = 30
FAIL           = "FAIL"                # bucket for bch_status < 0 (uncorrectable)

_POP = bytes(bin(i).count("1") for i in range(256))


def residual_bit_flips(sent_hex, recv_hex):
    """Bits still wrong AFTER decode (only meaningful on mismatched rows)."""
    try:
        a, b = bytes.fromhex(sent_hex), bytes.fromhex(recv_hex)
    except ValueError:
        return None
    if len(a) != len(b) or not a:
        return None
    return sum(_POP[x ^ y] for x, y in zip(a, b))


def collect_run_files(id_prefix):
    """Map run number N -> csv path for Test_*_<id_prefix>_<N>.csv."""
    rx = re.compile(rf"_{re.escape(id_prefix)}_(\d+)\.csv$", re.IGNORECASE)
    found = defaultdict(list)
    for path in sorted(glob.glob(os.path.join(OUTPUT_DIR, f"Test_*_{id_prefix}_*.csv"))):
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
    """Per-test tallies: counts/matches per decoder bucket + side stats."""
    counts   = defaultdict(int)   
    matches  = defaultdict(int)   
    crc_ok_n = defaultdict(int)   
    silent_bch = 0                # BCH claimed success, payload wrong
    silent_crc = 0                # BCH & CRC claimed success, payload wrong
    resid    = []                 
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
                    silent_bch += 1
                    if crc:
                        silent_crc += 1 # False Positive escaped both ECC and CRC!
                nf = residual_bit_flips(r.get("sent_word_hex", ""),
                                        r.get("corrected_word_hex", ""))
                if nf is not None:
                    resid.append(nf)

    return {"total": total, "counts": counts, "matches": matches,
            "crc_ok": crc_ok_n, "silent_bch": silent_bch, "silent_crc": silent_crc, "resid": resid}


def bucket_sort_key(b):
    return (1, 0) if b == FAIL else (0, b)


def fmt_pct(x):
    return "   --  " if (x is None or (isinstance(x, float) and math.isnan(x))) \
           else f"{x:7.2%}"


def mstd(values):
    """Mean and sample std (ddof=1) ignoring NaNs."""
    v = np.asarray(values, dtype=float)
    v = v[~np.isnan(v)]
    if v.size == 0:
        return float("nan"), float("nan")
    return float(np.mean(v)), float(np.std(v, ddof=1)) if v.size > 1 else 0.0


def process_campaign(n_inj):
    """Full analysis of the <n_inj>f campaign."""
    id_prefix = ID_TEMPLATE.format(n=n_inj)
    per_test_csv = os.path.join(OUTPUT_DIR, f"bch_mcu_{n_inj}f_per_test.csv")
    summary_csv  = os.path.join(OUTPUT_DIR, f"bch_mcu_{n_inj}f_summary.csv")
    plot_png     = os.path.join(OUTPUT_DIR, f"bch_mcu_{n_inj}f_plot.png")

    runs = collect_run_files(id_prefix)
    if not runs:
        print(f"\n### Campaign {n_inj} flips ('{id_prefix}'): no files found, skipped.")
        return None

    print(f"\n### Campaign {n_inj} injected flip(s) -- id '{id_prefix}' ###")
    per_run = {}
    missing = []
    for n in range(N_MIN, N_MAX + 1):
        path = runs.get(n)
        if path is None:
            missing.append(n)
            continue
        per_run[n] = analyze_run(path)
        
    if missing:
        print(f"  missing runs (skipped): {missing}")
    if not per_run:
        return None

    buckets = sorted({b for s in per_run.values() for b in s["counts"]},
                     key=bucket_sort_key)
    run_ids = sorted(per_run)

    # Per-test fractions and success rates
    frac = {b: [] for b in buckets}
    succ = {b: [] for b in buckets}
    word_ok = []                       
    for n in run_ids:
        s = per_run[n]
        for b in buckets:
            c = s["counts"].get(b, 0)
            frac[b].append(c / s["total"] if s["total"] else float("nan"))
            succ[b].append(s["matches"].get(b, 0) / c if c else float("nan"))
        word_ok.append(sum(s["matches"].values()) / s["total"]
                       if s["total"] else float("nan"))

    #  Pooled 
    pool_total   = sum(s["total"] for s in per_run.values())
    pool_counts  = {b: sum(s["counts"].get(b, 0)  for s in per_run.values()) for b in buckets}
    pool_matches = {b: sum(s["matches"].get(b, 0) for s in per_run.values()) for b in buckets}
    pool_crc     = {b: sum(s["crc_ok"].get(b, 0)  for s in per_run.values()) for b in buckets}
    pool_silent_bch = sum(s["silent_bch"] for s in per_run.values())
    pool_silent_crc = sum(s["silent_crc"] for s in per_run.values())
    pool_resid   = [x for s in per_run.values() for x in s["resid"]]

    stats = {}
    for b in buckets:
        fm, fs = mstd(frac[b])
        sm, ss = mstd(succ[b])
        stats[b] = {"frac_mean": fm, "frac_std": fs,
                    "succ_mean": sm, "succ_std": ss}

    os.makedirs(OUTPUT_DIR, exist_ok=True)

    #  per-test CSV -
    with open(per_test_csv, "w", newline="") as f:
        w = csv.writer(f)
        hdr = ["run", "words"]
        for b in buckets:
            hdr += [f"n_{b}", f"frac_{b}", f"bch_success_{b}"]
        hdr += ["payload_ok_rate", "silent_bch", "silent_crc"]
        w.writerow(hdr)
        for i, n in enumerate(run_ids):
            s   = per_run[n]
            row = [n, s["total"]]
            for b in buckets:
                c  = s["counts"].get(b, 0)
                fr, sr = frac[b][i], succ[b][i]
                row += [c,
                        f"{fr:.8f}" if not math.isnan(fr) else "",
                        f"{sr:.8f}" if not math.isnan(sr) else ""]
            row += [f"{word_ok[i]:.8f}", s["silent_bch"], s["silent_crc"]]
            w.writerow(row)

    #  summary CSV -
    with open(summary_csv, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["bucket", "pooled_words", "pooled_fraction",
                    "frac_mean", "frac_std",
                    "pooled_bch_success", "succ_mean", "succ_std",
                    "pooled_crc_ok_rate"])
        for b in buckets:
            c, st = pool_counts[b], stats[b]
            w.writerow([b, c,
                        f"{c / pool_total:.8f}",
                        f"{st['frac_mean']:.8f}", f"{st['frac_std']:.8f}",
                        f"{pool_matches[b] / c:.8f}" if c else "",
                        f"{st['succ_mean']:.8f}" if not math.isnan(st["succ_mean"]) else "",
                        f"{st['succ_std']:.8f}"  if not math.isnan(st["succ_std"])  else "",
                        f"{pool_crc[b] / c:.8f}" if c else ""])

    #  console summary 
    ok_m, ok_s = mstd(word_ok)
    print(f"\n  Wrote {per_test_csv}")
    print(f"  Wrote {summary_csv}")
    print("\n  " + "=" * 90)
    print(f"    {n_inj} INJECTED FLIP(S)  --  {len(run_ids)} tests, {pool_total} words")
    print("  " + "=" * 90)
    print(f"  {'decoder':>8} | {'words':>8} | {'fraction (mean+/-std)':>24} | "
          f"{'BCH success (mean+/-std)':>26} | {'CRC Valid Rate':>14}")
    print("  " + "-" * 90)
    for b in buckets:
        st = stats[b]
        lbl = str(b) if b != FAIL else ">=3/FAIL"
        f_str = f"{fmt_pct(st['frac_mean'])} +/- {st['frac_std']:6.2%}"
        s_str = (f"{fmt_pct(st['succ_mean'])} +/- {st['succ_std']:6.2%}"
                 if not math.isnan(st["succ_mean"]) else "      --")
        c_str = f"{fmt_pct(pool_crc[b]/pool_counts[b])}" if pool_counts[b] else "    --"
        print(f"  {lbl:>8} | {pool_counts[b]:>8} | {f_str:>24} | {s_str:>26} | {c_str:>14}")
    print("  " + "-" * 90)
    print(f"  Payload intact overall: {ok_m:.4%} +/- {ok_s:.4%}  (Target: >= 99.73%)")
    print(f"  ECC Silent Corruptions (BCH OK, Payload Wrong): {pool_silent_bch}")
    print(f"  CRITICAL False Positives (CRC OK, Payload Wrong): {pool_silent_crc}  (Target: 0)")
    if pool_resid:
        print(f"  Residual flipped bits on mismatched words: "
              f"mean {np.mean(pool_resid):.1f}, max {max(pool_resid)}")
    print("  " + "=" * 90)

    x      = np.arange(len(buckets))
    labels = [str(b) if b != FAIL else ">=3\n(FAIL)" for b in buckets]
    fmeans = [stats[b]["frac_mean"] for b in buckets]
    fstds  = [stats[b]["frac_std"]  for b in buckets]
    smeans = [stats[b]["succ_mean"] for b in buckets]
    sstds  = [stats[b]["succ_std"]  for b in buckets]
    cmeans = [pool_crc[b]/pool_counts[b] if pool_counts[b] else 0.0 for b in buckets]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4.5))
    fig.suptitle(f"BCH logic only (no flash) -- {n_inj} injected flip(s), "
                 f"{len(run_ids)} tests, {pool_total} words (mean ± std)",
                 fontsize=12)

    ax1.bar(x, fmeans, yerr=fstds, capsize=4, color="#1A5F9E", alpha=0.85)
    ax1.set_xticks(x); ax1.set_xticklabels(labels)
    ax1.set_xlabel("Corrections reported by decoder")
    ax1.set_ylabel("Fraction of words [-]")
    ax1.set_title("Decoder verdict distribution")
    ax1.grid(True, axis="y", linestyle="--", linewidth=0.5, alpha=0.6)
    for xi, m in zip(x, fmeans):
        if not math.isnan(m):
            ax1.annotate(f"{m:.2%}", (xi, m), ha="center", va="bottom",
                         fontsize=8, xytext=(0, 3), textcoords="offset points")

    ok_idx = [i for i, m in enumerate(smeans) if not math.isnan(m)]
    ax2.bar([x[i] for i in ok_idx], [smeans[i] for i in ok_idx],
            yerr=[sstds[i] for i in ok_idx], capsize=4,
            color="#2E8B57", alpha=0.85)
    ax2.set_xticks(x); ax2.set_xticklabels(labels)
    ax2.set_ylim(0, 1.05)
    ax2.set_xlabel("Corrections reported by decoder")
    ax2.set_ylabel("Payload intact after decode [-]")
    ax2.set_title("BCH success rate per verdict")
    ax2.grid(True, axis="y", linestyle="--", linewidth=0.5, alpha=0.6)
    for i in ok_idx:
        ax2.annotate(f"{smeans[i]:.2%}", (x[i], smeans[i]), ha="center",
                     va="bottom", fontsize=8, xytext=(0, 3),
                     textcoords="offset points")

    fig.tight_layout()
    fig.savefig(plot_png, dpi=150)
    print(f"  Saved plot -> {plot_png}")

    return {"n_inj": n_inj, "tests": len(run_ids), "total": pool_total,
            "buckets": buckets, "stats": stats, "crc_means": cmeans,
            "ok_mean": ok_m, "ok_std": ok_s, 
            "silent_bch": pool_silent_bch, "silent_crc": pool_silent_crc}


def overview_plot(results):
    """Cross-campaign figure: how the BCH responds as injected flips grow."""
    png = os.path.join(OUTPUT_DIR, "bch_mcu_overview.png")
    ns  = [r["n_inj"] for r in results]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4.5))
    fig.suptitle("BCH logic vs number of injected bit-flips (no flash in loop)",
                 fontsize=12)

    ax1.errorbar(ns, [r["ok_mean"] for r in results],
                 yerr=[r["ok_std"] for r in results],
                 marker="o", capsize=4, color="#2E8B57", linewidth=1.8)
    ax1.set_xticks(ns)
    ax1.set_ylim(-0.05, 1.05) 
    ax1.set_xlabel("Artificially injected bit-flips per word")
    ax1.set_ylabel("Payload intact after decode [-]")
    ax1.set_title("End-to-end success rate")
    ax1.grid(True, linestyle="--", linewidth=0.5, alpha=0.3)

    all_buckets = sorted({b for r in results for b in r["buckets"]},
                         key=bucket_sort_key)
    width  = 0.8 / len(all_buckets)
    colors = plt.cm.viridis(np.linspace(0.15, 0.85, len(all_buckets)))
    
    for j, b in enumerate(all_buckets):
        raw_vals = [r["stats"].get(b, {}).get("frac_mean", 0.0) or 0.0 for r in results]
        raw_errs = [r["stats"].get(b, {}).get("frac_std", 0.0) or 0.0 for r in results]

        vals = [v if v > 0 else np.nan for v in raw_vals]
        errs = [e if v > 0 else np.nan for v, e in zip(raw_vals, raw_errs)]
        
        pos  = [n + (j - (len(all_buckets) - 1) / 2) * width for n in ns]
        lbl  = f"reported {b}" if b != FAIL else "FAIL (>=3)"
        
        ax2.bar(pos, vals, width=width, yerr=errs, capsize=2,
                color=colors[j], label=lbl, log=True, bottom=1e-4)

    ax2.set_xticks(ns)
    ax2.set_yscale("log")
    ax2.set_ylim(bottom=1e-4, top=3.0) 
    ax2.set_xlabel("Artificially injected bit-flips (Macro-category)")
    ax2.set_ylabel("Fraction of words [-]")
    ax2.set_title("Decoder verdict mix")
    
    for i in range(len(ns) - 1):
        mid_point = (ns[i] + ns[i+1]) / 2.0
        ax2.axvline(x=mid_point, color='gray', linestyle=':', linewidth=1.0, alpha=0.7)

    ax2.grid(True, which="both", axis="y", linestyle="--", linewidth=0.5, alpha=0.3)
    
    ax2.legend(title="BCH Decoder Verdict", fontsize=8, title_fontsize=9, loc="lower left")

    fig.tight_layout()
    fig.savefig(png, dpi=150)
    print(f"\nSaved overview plot -> {png}")


def main():
    print(f"Scanning '{OUTPUT_DIR}' for campaigns "
          f"{[ID_TEMPLATE.format(n=n) for n in INJECTED_FLIPS]}, "
          f"runs {N_MIN}..{N_MAX}")

    results = []
    for n_inj in INJECTED_FLIPS:
        r = process_campaign(n_inj)
        if r is not None:
            results.append(r)

    if not results:
        print("\nNo campaign produced data -- nothing to plot.")
        return

    # Cross-campaign recap table
    print("\n" + "=" * 85)
    print("  CROSS-CAMPAIGN RECAP (Target: 3σ / 99.73% Data Match | 0 False Positives)")
    print("=" * 85)
    for r in results:
        print(f"  {r['n_inj']} flips: Match {r['ok_mean']:8.4%} +/- {r['ok_std']:7.4%} | "
              f"ECC Silent: {r['silent_bch']:<5} | CRC False Positives: {r['silent_crc']:<5}")
    print("=" * 85)

    if len(results) > 1:
        overview_plot(results)

    plt.show()


if __name__ == "__main__":
    main()