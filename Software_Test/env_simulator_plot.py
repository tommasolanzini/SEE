import matplotlib.pyplot as plt
import pandas as pd

runs = pd.read_csv(r".\Software_Test\runs_augmented.csv")
total_memory = 2 * 42 * 1e9     # bytes
memory_fraction = 0.005
mission_lifetime = 2            # years
n_test = len(runs)
# Uncorrectables rate prediction
plt.figure(figsize=(7,4))
unc_rate = runs['uncorrectables'] / (8 * total_memory * memory_fraction * mission_lifetime * 365 * 24 * 3600)
plt.hist(unc_rate, bins=40, density=True, alpha=0.6)
plt.axvline(unc_rate.mean(), color='red', linestyle='--', label=f'Mean: {unc_rate.mean():.3e} /bit/s')
plt.xlabel(r"Uncorrectable Errors Rate [$bit^{-1}s^{-1}$]")
plt.ylabel("Probability Density")
plt.title(f"Distribution of Uncorrectable Rates ({n_test} runs)")
plt.legend()
plt.show()

print(f"Mean Uncorrectable Rate: {unc_rate.mean():.3e} /bit/s")
print(f"Standard Deviation of Uncorrectable Rate: {unc_rate.std():.3e} /bit/s")