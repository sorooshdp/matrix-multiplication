import matplotlib.pyplot as plt

# Specified minimum runtimes (in seconds)
mins = {
    'C': 0.040733,
    'C++': 0.0513986,
    'Rust': 0.0767405,
    'Java': 0.1943766,
    'JavaScript': 0.358764,
    'Python': 24.479005
}

# Prepare plot
fig, ax = plt.subplots(figsize=(8, 6))
bars = ax.bar(mins.keys(), mins.values(), color='orange')

# Logarithmic y-axis
ax.set_yscale('log')

# Remove y-axis ticks and labels
ax.yaxis.set_ticks([])

# Grid lines on major scale ticks
ax.yaxis.grid(True, which='major', linestyle='--', linewidth=0.5)

# Annotate bars with values in milliseconds (s * 1000)
for bar in bars:
    sec = bar.get_height()
    ms = sec * 1000
    ax.annotate(f'{ms:.3f} ms',
                xy=(bar.get_x() + bar.get_width() / 2, sec),
                xytext=(0, 5), textcoords='offset points',
                ha='center', va='bottom', fontsize=10)

# Labels and title
ax.set_xlabel('Programming Language', fontsize=12)
ax.set_ylabel('Minimum Time (s) [log scale]', fontsize=12)
ax.set_title('Minimum Matrix Multiplication Runtimes (Log Scale)', fontsize=14)

plt.tight_layout()
plt.savefig('matrix_mul_min_runtimes_log_final.png', dpi=300)
plt.show()
