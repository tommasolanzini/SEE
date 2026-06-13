import numpy as np
from tqdm import trange

#  Knowns
mission_lifetime = 2            # years
word_size = 512 + 69            # bytes
total_memory = 2 * 42 * 1e9     # bytes
spenvis_rate = 6e-12            # errors per bit per second


#  Parameters
scrubbing_frequency = 5         # days
memory_fraction = 0.01
total_memory *= memory_fraction  # Use only a fraction of the total memory for the simulation

num_words = int(total_memory / word_size)
t = scrubbing_frequency * 24 * 3600 
lam = spenvis_rate * 8 * word_size * t # errors per word per scrubbing period
n_scrub = int(mission_lifetime * 365 / scrubbing_frequency)
uncorrectables = 0
errors = np.zeros(num_words)
errors_nombu = np.zeros(num_words)

batch_size = 10_000
runs = 100
write = True
cycles = trange(runs, desc="Simulating runs")
for run in cycles:
    uncorrectables = 0
    for n in range(n_scrub):
        for start in range(0, num_words, batch_size):
            end = min(start + batch_size, num_words)
            k = np.random.poisson(lam, end - start)
            extra = np.random.choice([0,1,2], size=(end-start, k.max()), p=[0.8,0.15,0.05])
            mask = np.arange(k.max()) < k[:, None]
            k += (extra * mask).sum(axis=1)
            single_fail = np.sum(k == 1)
            uncorrectables += np.sum(np.random.random(size=np.sum(k == 1)) > 0.999797)
            uncorrectables += np.sum(np.random.random(size=np.sum(k == 2)) > 0.999530)
            uncorrectables += np.sum(k > 2)

    # Postfix the uncorretable rate to the tqdm progress bar
    cycles.set_postfix({"U.R. (per bit per second)": f"{uncorrectables / (total_memory * 8 * mission_lifetime * 365 * 24 * 3600):.5e}"})

    # Save errors, errors_nombu, generated_rate, uncorrectables, fails_bit on the runs.csv file
    if write:
        with open(r".\Software_Test\runs_augmented.csv", "a") as f:
            f.write(f"\n{uncorrectables}")