# import matplotlib.pyplot as plt

# # Updated peak memory usages (in KB)
# peak_memory = {
#     'Rust': 2568,
#     'C': 3460,
#     'Java': 3953,
#     'C++': 3976,
#     'JavaScript': 4024,
#     'Python': 37128
# }

# # Prepare plot
# fig, ax = plt.subplots(figsize=(8, 6))
# bars = ax.bar(peak_memory.keys(), peak_memory.values(), color='orange')

# # Logarithmic y-axis
# ax.set_yscale('log')

# # Remove y-axis ticks and labels
# ax.yaxis.set_ticks([])

# # Grid lines on major scale ticks
# ax.yaxis.grid(True, which='major', linestyle='--', linewidth=0.5)

# # Annotate bars with values in KB
# for bar in bars:
#     kb = bar.get_height()
#     ax.annotate(f'{kb:.0f} KB',
#                 xy=(bar.get_x() + bar.get_width() / 2, kb),
#                 xytext=(0, 5), textcoords='offset points',
#                 ha='center', va='bottom', fontsize=10)

# # Labels and title
# ax.set_xlabel('Programming Language', fontsize=12)
# ax.set_ylabel('Peak Memory Usage (KB) [log scale]', fontsize=12)
# ax.set_title('Peak Memory Usage for Matrix Multiplication', fontsize=14)

# plt.tight_layout()
# plt.savefig('matrix_mul_peak_memory_log_updated.png', dpi=300)
# plt.show()

# version 2

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Rectangle

# Configure matplotlib for publication-quality figures
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

# Peak memory usage data organized by performance tier
peak_memory = {
    'Rust': 2568,
    'C': 3460,
    'Java': 3953,
    'C++': 3976,
    'JavaScript': 4024,
    'Python': 37128
}

# Define color palette for academic publications (colorblind-friendly)
colors = {
    'Rust': '#2E86AB',      # Professional blue
    'C': '#A23B72',         # Deep magenta
    'Java': '#F18F01',      # Warm orange
    'C++': '#C73E1D',       # Strong red
    'JavaScript': '#592A2A', # Dark brown
    'Python': '#F4A261'     # Light orange
}

# Create figure with appropriate size for academic papers
fig, ax = plt.subplots(figsize=(10, 6))

# Extract languages and values for plotting
languages = list(peak_memory.keys())
values = list(peak_memory.values())
bar_colors = [colors[lang] for lang in languages]

# Create bar chart with enhanced styling
bars = ax.bar(languages, values, 
              color=bar_colors,
              edgecolor='black',
              linewidth=0.8,
              alpha=0.85)

# Configure logarithmic y-axis with proper tick formatting
ax.set_yscale('log')
ax.set_ylim(1000, 50000)  # Set appropriate limits for better visualization

# Create custom y-axis ticks and labels
y_ticks = [1000, 2000, 5000, 10000, 20000, 50000]
ax.set_yticks(y_ticks)
ax.set_yticklabels([f'{tick:,}' for tick in y_ticks])

# Add subtle grid for better readability
ax.grid(True, which='major', axis='y', linestyle='--', 
        linewidth=0.5, alpha=0.7, color='gray')
ax.set_axisbelow(True)  # Place grid behind bars

# Add value annotations with improved formatting
for i, (bar, value) in enumerate(zip(bars, values)):
    # Position annotation above each bar
    ax.annotate(f'{value:,} KB',
                xy=(bar.get_x() + bar.get_width() / 2, value),
                xytext=(0, 8),
                textcoords='offset points',
                ha='center', va='bottom',
                fontsize=10,
                fontweight='bold',
                color='black')

# Calculate and display memory efficiency ratio
min_memory = min(values)
for i, (lang, value) in enumerate(peak_memory.items()):
    ratio = value / min_memory
    if ratio > 1:
        ax.annotate(f'{ratio:.1f}×',
                    xy=(i, value * 0.7),
                    ha='center', va='center',
                    fontsize=9,
                    color='white',
                    fontweight='bold',
                    bbox=dict(boxstyle='round,pad=0.3', 
                             facecolor='black', alpha=0.7))

# Enhanced labels and title
ax.set_xlabel('Programming Language', fontsize=14, fontweight='bold')
ax.set_ylabel('Peak Memory Usage (KB)', fontsize=14, fontweight='bold')
ax.set_title('Comparative Analysis of Peak Memory Usage\nfor Matrix Multiplication Operations', 
             fontsize=16, fontweight='bold', pad=20)

# Add subtle background highlighting for different performance tiers
# Low memory tier (Rust, C)
low_memory_rect = Rectangle((-0.5, 1000), 2, 49000, 
                           facecolor='lightgreen', alpha=0.1, zorder=0)
ax.add_patch(low_memory_rect)

# Medium memory tier (Java, C++, JavaScript)
medium_memory_rect = Rectangle((1.5, 1000), 3, 49000, 
                              facecolor='lightyellow', alpha=0.1, zorder=0)
ax.add_patch(medium_memory_rect)

# High memory tier (Python)
high_memory_rect = Rectangle((4.5, 1000), 1, 49000, 
                            facecolor='lightcoral', alpha=0.1, zorder=0)
ax.add_patch(high_memory_rect)

# Add performance tier labels
ax.text(0.5, 45000, 'Low Memory\nTier', ha='center', va='center', 
        fontsize=10, style='italic', alpha=0.6)
ax.text(3, 45000, 'Medium Memory Tier', ha='center', va='center', 
        fontsize=10, style='italic', alpha=0.6)
ax.text(5, 45000, 'High Memory\nTier', ha='center', va='center', 
        fontsize=10, style='italic', alpha=0.6)

# Add statistical summary as text box
# stats_text = f"""Statistical Summary:
# • Range: {min(values):,} - {max(values):,} KB
# • Mean: {np.mean(values):,.0f} KB
# • Median: {np.median(values):,.0f} KB
# • Std Dev: {np.std(values):,.0f} KB"""

# ax.text(0.98, 0.98, stats_text, transform=ax.transAxes,
#         verticalalignment='top', horizontalalignment='right',
#         bbox=dict(boxstyle='round,pad=0.5', facecolor='lightgray', alpha=0.8),
#         fontsize=9, fontfamily='monospace')

# Improve layout and save with high quality
plt.tight_layout()
plt.subplots_adjust(bottom=0.12, top=0.88)

# Save the figure in multiple formats for academic use
plt.savefig('memory_usage_analysis_academic.png', dpi=300, bbox_inches='tight')
plt.savefig('memory_usage_analysis_academic.pdf', bbox_inches='tight')  # Vector format for publications
plt.savefig('memory_usage_analysis_academic.svg', bbox_inches='tight')  # Scalable vector format

plt.show()

# Generate additional analysis
print("Memory Usage Analysis Summary:")
print("=" * 40)
for lang, mem in sorted(peak_memory.items(), key=lambda x: x[1]):
    efficiency = min(peak_memory.values()) / mem
    print(f"{lang:12}: {mem:6,} KB (Efficiency: {efficiency:.3f})")
print("=" * 40)
print(f"Memory span: {max(peak_memory.values()) / min(peak_memory.values()):.1f}× difference")
print(f"Languages within 2× of minimum: {sum(1 for v in peak_memory.values() if v <= 2 * min(peak_memory.values()))}/{len(peak_memory)}")