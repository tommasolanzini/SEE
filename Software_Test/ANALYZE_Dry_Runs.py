"""
ANALYZE_DR_FLIPS.py

Aggregates the per-word results of the DryRun (DR) link-integrity campaign.
Calculates error distributions across runs, prints mean/std dev percentages, 
and displays a boxplot natively.
"""

import os
import re
import csv
import glob
from collections import defaultdict

try:
    import matplotlib.pyplot as plt
    import matplotlib.patches as mpatches
    from matplotlib.lines import Line2D
    import numpy as np
except ImportError:
    print("ERROR: This script requires matplotlib and numpy.")
    print("Run: pip install matplotlib numpy")
    exit(1)

# --- Configuration ----------------------------------------------------------
OUTPUT_DIR = os.path.join(".", "Software_Test", "Test_Output")
ID_PREFIX  = "DR"          # test IDs are DR_1 .. DR_<N_MAX>
N_MIN      = 1
N_MAX      = 30
SHORT_FLAG = -1            # sentinel kept in the matrix for SHORT / unusable rows

MATRIX_CSV = os.path.join(OUTPUT_DIR, "flips_matrix_DR.csv")
DIST_CSV   = os.path.join(OUTPUT_DIR, "flips_distribution_DR.csv")

# Per-byte popcount lookup
_POP = bytes(bin(i).count("1") for i in range(256))


def bit_flips(sent_hex, recv_hex):
    """Exact number of differing BITS between two hex strings."""
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
    """Map run number -> csv path for every  Test_*_DR_<n>.csv  in OUTPUT_DIR."""
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
            print(f"  WARNING: DR_{n} matches {len(paths)} files; using latest.")
        runs[n] = paths[-1]
    return runs


def main():
    runs = collect_run_files()
    if not runs:
        print(f"No DR campaign files found in '{OUTPUT_DIR}'.")
        return

    flips_by_word = defaultdict(dict)
    
    # Track distributions PER RUN for the boxplot and std dev math
    dist_per_run = {n: defaultdict(int) for n in range(N_MIN, N_MAX + 1)}
    total_per_run = {n: 0 for n in range(N_MIN, N_MAX + 1)}
    
    short_rows = 0
    total_obs  = 0
    max_word   = 0
    present_runs = []

    print(f"Scanning '{OUTPUT_DIR}' for DR_{N_MIN}..DR_{N_MAX} ...")
    for n in range(N_MIN, N_MAX + 1):
        path = runs.get(n)
        if path is None:
            continue
        present_runs.append(n)

        with open(path, newline="") as f:
            reader = csv.DictReader(f)
            for r in reader:
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
                dist_per_run[n][nf] += 1
                total_per_run[n] += 1
                total_obs += 1

    if total_obs == 0:
        print("\nNo usable word-observations found.")
        return

    # --- Math: Mean and Std Dev across the tests ---
    pct_0 = []
    pct_1 = []
    pct_2 = []
    pct_3 = []
    pct_m = []

    for n in present_runs:
        tot = total_per_run[n]
        if tot == 0: continue
        
        c0 = dist_per_run[n].get(0, 0)
        c1 = dist_per_run[n].get(1, 0)
        c2 = dist_per_run[n].get(2, 0)
        c3 = dist_per_run[n].get(3, 0)
        cm = sum(c for k, c in dist_per_run[n].items() if k >= 3)

        pct_0.append((c0 / tot) * 100)
        pct_1.append((c1 / tot) * 100)
        pct_2.append((c2 / tot) * 100)
        pct_3.append((c3 / tot) * 100)
        pct_m.append((cm / tot) * 100)

    mean_0, std_0 = np.mean(pct_0), np.std(pct_0)
    mean_1, std_1 = np.mean(pct_1), np.std(pct_1)
    mean_2, std_2 = np.mean(pct_2), np.std(pct_2)
    mean_3, std_3 = np.mean(pct_3), np.std(pct_3)
    mean_m, std_m = np.mean(pct_m), np.std(pct_m)

    print("\n" + "=" * 60)
    print(f"  INJECTED ERRORS STATISTICS (MEAN ACROSS {len(present_runs)} TESTS)")
    print("=" * 60)
    print(f"{mean_0:>6.2f}% +- {std_0:<5.2f}%  have 0 errors")
    print(f"{mean_1:>6.2f}% +- {std_1:<5.2f}%  have 1 error")
    print(f"{mean_2:>6.2f}% +- {std_2:<5.2f}%  have 2 errors")
    print(f"{mean_3:>6.2f}% +- {std_3:<5.2f}%  have 3 errors")
    print(f"{mean_m:>6.2f}% +- {std_m:<5.2f}%  have >2 errors")
    print("=" * 60 + "\n")

    # --- Display Boxplot Natively ---
    # Define our bins to prevent x-axis crowding
    labels = ["0", "1", "2", "3", "4", ">= 5"]
    
    # Initialize an empty list for each bin to hold the counts from each test
    data_for_box = [[] for _ in labels]

    for n in present_runs:
        c0 = dist_per_run[n].get(0, 0)
        c1 = dist_per_run[n].get(1, 0)
        c2 = dist_per_run[n].get(2, 0)
        c3 = dist_per_run[n].get(3, 0)
        c4 = dist_per_run[n].get(4, 0)
        c5_plus = sum(c for k, c in dist_per_run[n].items() if k >= 5)

        # Append this run's data into the respective bins
        data_for_box[0].append(c0)
        data_for_box[1].append(c1)
        data_for_box[2].append(c2)
        data_for_box[3].append(c3)
        data_for_box[4].append(c4)
        data_for_box[5].append(c5_plus)

    plt.figure(figsize=(10, 6))
    
    # Create the boxplot
    box = plt.boxplot(data_for_box, positions=range(len(labels)), 
                      patch_artist=True, showmeans=True, 
                      meanline=False, 
                      boxprops=dict(facecolor="#ADD8E6", color="black"))
    
    plt.xticks(range(len(labels)), labels)
    
    # Only use log scale if there is actually data, otherwise it might throw a warning
    plt.yscale("log") 
    
    plt.title("Distribution of Bit-Flips per Word across Tests", fontsize=14, fontweight="bold")
    plt.xlabel("Errors per Word (k)", fontsize=12)
    plt.ylabel("Number of Words (Log Scale)", fontsize=12)
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    
    # --- Add Legend ---
    legend_elements = [
        mpatches.Patch(facecolor="#ADD8E6", edgecolor="black", label="IQR (25th - 75th Percentile)"),
        Line2D([0], [0], color="orange", lw=2, label="Median"),
        Line2D([0], [0], marker="^", color="green", label="Mean", markerfacecolor="green", markersize=8, linestyle="None"),
        Line2D([0], [0], marker="o", color="black", label="Outliers", markerfacecolor="white", markersize=5, linestyle="None")
    ]
    plt.legend(handles=legend_elements, loc="upper right")
    
    # Adjust layout so nothing gets cut off
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()