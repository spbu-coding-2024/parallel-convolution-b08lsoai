import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

modes = []
data = []

filename = "result_small"

def read_benchmark_data_pandas(filename):
    df = pd.read_csv(filename + ".csv", comment='#')
    
    modes = df['mode'].tolist()
    value_cols = [col for col in df.columns if col != 'mode']
    data = [df[col].tolist() for col in value_cols]
    data = list(zip(*data))  
    
    return modes, data

modes, data = read_benchmark_data_pandas(filename)

fig, ax = plt.subplots(figsize=(8, 6))

bp = ax.boxplot(
    data,
    positions=np.arange(len(data)) + 1,
    widths=0.6,
    patch_artist=True,
    showfliers=False,
)

colors = ["#FF9999", "#99FF99", "#9999FF", "#FFCC99", "#CC99FF", "#66CCCC"]
for patch, color in zip(bp["boxes"], colors * 10):
    patch.set_facecolor(color)
    patch.set_alpha(0.6)

for i, d in enumerate(data, start=1):
    x = np.random.normal(i, 0.04, size=len(d))
    ax.plot(x, d, "o", markersize=5, alpha=0.3, color="black")

ax.set_xticks(np.arange(1, len(modes) + 1))
ax.set_xticklabels(modes, rotation=20)
ax.set_ylabel("Time (sec)")
ax.set_title("Benchmark: motion (9x9) | 736x469")

plt.grid(True, axis="y", linestyle="--", alpha=0.6)
plt.tight_layout()

plt.savefig(filename + ".png", dpi=200)
plt.show()

print("\n=== Benchmark Summary ===")
print(f"{'Mode':<10} {'Min':<10} {'Median':<10} {'Max':<10}")
print("-" * 50)
for i, mode in enumerate(modes):
    vals = data[i]
    print(f"{mode:<10} {np.min(vals):<10.6f} {np.median(vals):<10.6f} {np.max(vals):<10.6f}")