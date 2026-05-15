import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

os.makedirs("./benchmark_res", exist_ok=True)

csv_files = sorted([f for f in os.listdir("./benchmark_res") if f.endswith('.csv')])

labels = ["1 file", "3 files", "10 files", "20 files", "50 files"]

standard_vals = []
queue_vals = []
standard_stats = []
queue_stats = []

for f in csv_files:
    df = pd.read_csv(os.path.join("./benchmark_res", f), comment='#')
    
    standard_row = df[df['mode'] == 'standart'].iloc[0]
    queue_row = df[df['mode'] == 'queue'].iloc[0]
    
    standard_vals.append(standard_row['avg'])
    queue_vals.append(queue_row['avg'])
    
    run_cols = [col for col in df.columns if col.startswith('run_')]
    standard_runs = [standard_row[col] for col in run_cols]
    queue_runs = [queue_row[col] for col in run_cols]
    
    standard_stats.append({
        'min': np.min(standard_runs),
        'max': np.max(standard_runs),
        'median': np.median(standard_runs)
    })
    queue_stats.append({
        'min': np.min(queue_runs),
        'max': np.max(queue_runs),
        'median': np.median(queue_runs)
    })

x = range(len(csv_files))
bar_width = 0.35

fig, ax = plt.subplots(figsize=(6, 8))
ax.set_yscale('log')
ax.bar([i - bar_width/2 for i in x], standard_vals, width=bar_width, 
       label='Standard', color="#6082EA")
ax.bar([i + bar_width/2 for i in x], queue_vals, width=bar_width, 
       label='Queue', color="#E4678F")

ax.set_xticks(x)
ax.set_xticklabels(labels, rotation=30, ha="right")
ax.set_ylabel("Time (seconds)")
ax.set_title("Standard vs Queue Mode")
ax.legend()
ax.grid(axis="y", linestyle="--", alpha=0.7)

plt.tight_layout()
plt.savefig(os.path.join("./benchmark_res", "standard_vs_queue.png"), dpi=200)
plt.show()

print("\n" + "="*90)
print("BENCHMARK SUMMARY")
print("="*90)

for i, label in enumerate(labels):
    print(f"\n{label}:")
    print("-"*50)
    print(f"{'Mode':<10} {'Avg (sec)':<12} {'Min (sec)':<12} {'Median (sec)':<12} {'Max (sec)':<12}")
    print("-"*50)
    
    print(f"{'Standard':<10} {standard_vals[i]:<12.4f} {standard_stats[i]['min']:<12.4f} {standard_stats[i]['median']:<12.4f} {standard_stats[i]['max']:<12.4f}")
    print(f"{'Queue':<10} {queue_vals[i]:<12.4f} {queue_stats[i]['min']:<12.4f} {queue_stats[i]['median']:<12.4f} {queue_stats[i]['max']:<12.4f}")