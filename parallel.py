import matplotlib.pyplot as plt
import numpy as np

# Configure matplotlib for clean, professional figures
plt.rcParams.update({
    'font.size': 12,
    'font.family': 'serif',
    'axes.linewidth': 1.2,
    'axes.spines.top': False,
    'axes.spines.right': False,
    'xtick.major.size': 6,
    'ytick.major.size': 6,
    'legend.frameon': False,
    'figure.dpi': 300
})

# Define the benchmark data from parallel processing results
languages = ['C', 'C++', 'Rust', 'Java', 'JS', 'Python']
best_times = [0.003, 0.0035628, 0.005766, 0.0573497, 0.168085, 8.160835]  # in seconds

# Memory usage data (initial + peak increase)
initial_memory = [2.86, 3.036, 2.568, 0.947, 3.851, 11.776]  # in MB
memory_increase = [0, 0, 0.024, 3.503, 0, 20.808]  # in MB
total_memory = [init + inc for init, inc in zip(initial_memory, memory_increase)]

# Define yellow color palette with different shades
yellow_colors = [
    '#FFF59D',      # Light yellow
    '#FFEE58',      # Medium light yellow
    '#FFEB3B',      # Standard yellow
    '#FDD835',      # Medium yellow
    '#F9A825',      # Dark yellow
    '#F57F17'       # Deep yellow
]

# Create speed/performance chart
def create_speed_chart():
    fig, ax = plt.subplots(figsize=(10, 6))
    
    # Create bar chart
    bars = ax.bar(languages, best_times, 
                  color=yellow_colors,
                  edgecolor='black',
                  linewidth=0.8,
                  alpha=0.85)
    
    # Configure logarithmic y-axis
    ax.set_yscale('log')
    
    # Add subtle grid
    ax.grid(True, which='major', axis='y', linestyle='--', 
            linewidth=0.5, alpha=0.7, color='gray')
    ax.set_axisbelow(True)
    
    # Add value annotations
    for bar, time in zip(bars, best_times):
        height = bar.get_height()
        if time < 1:
            label = f'{time:.4f}s'
        else:
            label = f'{time:.2f}s'
        ax.annotate(label,
                    xy=(bar.get_x() + bar.get_width() / 2, height),
                    xytext=(0, 8),
                    textcoords='offset points',
                    ha='center', va='bottom',
                    fontsize=10,
                    fontweight='bold',
                    color='black')
    
    # Labels and title
    ax.set_xlabel('Programming Language', fontsize=14, fontweight='bold')
    ax.set_ylabel('Execution Time (seconds)', fontsize=14, fontweight='bold')
    ax.set_title('Parallel Matrix Multiplication Performance\n(500×500 matrices, 8 threads)', 
                 fontsize=16, fontweight='bold', pad=20)
    
    plt.tight_layout()
    plt.savefig('parallel_speed_chart.png', dpi=300, bbox_inches='tight')
    plt.savefig('parallel_speed_chart.pdf', bbox_inches='tight')
    plt.show()

# Create memory usage chart
def create_memory_chart():
    fig, ax = plt.subplots(figsize=(10, 6))
    
    # Create simple bar chart for total memory usage
    bars = ax.bar(languages, total_memory, 
                  color=yellow_colors,
                  edgecolor='black',
                  linewidth=0.8,
                  alpha=0.85)
    
    # Add subtle grid
    ax.grid(True, which='major', axis='y', linestyle='--', 
            linewidth=0.5, alpha=0.7, color='gray')
    ax.set_axisbelow(True)
    
    # Add value labels for total memory usage
    for bar, total in zip(bars, total_memory):
        ax.annotate(f'{total:.2f} MB',
                    xy=(bar.get_x() + bar.get_width() / 2, total),
                    xytext=(0, 8),
                    textcoords='offset points',
                    ha='center', va='bottom',
                    fontsize=10,
                    fontweight='bold',
                    color='black')
    
    # Labels and title
    ax.set_xlabel('Programming Language', fontsize=14, fontweight='bold')
    ax.set_ylabel('Memory Usage (MB)', fontsize=14, fontweight='bold')
    ax.set_title('Memory Usage for Parallel Matrix Multiplication\n(500×500 matrices, 8 threads)', 
                 fontsize=16, fontweight='bold', pad=20)
    
    plt.tight_layout()
    plt.savefig('parallel_memory_chart.png', dpi=300, bbox_inches='tight')
    plt.savefig('parallel_memory_chart.pdf', bbox_inches='tight')
    plt.show()

# Generate both charts
print("Generating Parallel Processing Speed Chart...")
create_speed_chart()

print("Generating Parallel Processing Memory Chart...")
create_memory_chart()

# Generate analysis summary
print("\n=== PARALLEL PROCESSING BENCHMARK SUMMARY ===")
print("=" * 55)

print("Performance Rankings (Best Time):")
sorted_perf = sorted(zip(languages, best_times), key=lambda x: x[1])
for i, (lang, time) in enumerate(sorted_perf, 1):
    print(f"{i}. {lang:12}: {time:.6f}s")

print("\nMemory Usage Rankings (Total Memory):")
sorted_mem = sorted(zip(languages, total_memory), key=lambda x: x[1])
for i, (lang, mem) in enumerate(sorted_mem, 1):
    print(f"{i}. {lang:12}: {mem:.2f} MB")

print("\nKey Insights:")
print(f"• C achieved the best performance at {best_times[0]:.6f}s")
print(f"• Python was {best_times[-1]/best_times[0]:.1f}x slower than C")
print(f"• Rust used the least memory at {total_memory[2]:.2f} MB")
print(f"• Python used {total_memory[-1]/total_memory[2]:.1f}x more memory than Rust")
print(f"• C/C++ showed minimal memory overhead during execution")

print(f"\n=== DETAILED PERFORMANCE TABLE ===")
print(f"{'Language':<12} {'Time (s)':<12} {'Memory (MB)':<12} {'Speedup':<10}")
print("-" * 50)
baseline_time = best_times[0]  # Use C as baseline
for lang, time, mem in zip(languages, best_times, total_memory):
    speedup = baseline_time / time
    print(f"{lang:<12} {time:<12.6f} {mem:<12.2f} {speedup:<10.2f}x")

print("=" * 55)
print(f"Speed span: {max(best_times) / min(best_times):.1f}× difference")
print(f"Memory span: {max(total_memory) / min(total_memory):.1f}× difference")

print("\nChart files generated:")
print("- parallel_speed_chart.png and parallel_speed_chart.pdf")
print("- parallel_memory_chart.png and parallel_memory_chart.pdf")