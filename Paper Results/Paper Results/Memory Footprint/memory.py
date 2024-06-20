import matplotlib.pyplot as plt

# Data initialization
ram_types = ['ROM', 'RAM']
# These data has been extracted from cosy analysis tool
# You can get by run in //UDP-Protocol-using-Femto-Containers/UDP_hook_static:
    # make cosy and browse the webpage of the tool 
gnrc_values = [95257, 13976]  # gnrc values in bytes
fc_values = [97630, 13176]  # FC values in bytes

# Convert byte values to kilobytes
gnrc_values_kb = [x / 1024 for x in gnrc_values]
fc_values_kb = [x / 1024 for x in fc_values]

# Define bar width and index for bars
bar_width = 0.35
index = range(len(ram_types))

# Create a figure and axis for the plot
fig, ax = plt.subplots(figsize=(10, 8))

# Plot FC values first (in blue) and GNRC values second (in red)
# FC bars in blue with a different hatch pattern
bars2 = ax.bar(index, fc_values_kb, bar_width, label='FC', color='blue', edgecolor='black')

# GNRC bars in red with a hatch pattern
bars1 = ax.bar([p + bar_width for p in index], gnrc_values_kb, bar_width, label='GNRC', color='#DAF7A6', edgecolor='black', hatch='/')

# Set labels and title for the plot
ax.set_ylabel('Memory (KB)', fontsize=16)
ax.set_title('Memory Comparison', fontsize=18)
ax.set_xticks([p + bar_width / 2 for p in index])
ax.set_xticklabels(ram_types, fontsize=16)
ax.legend(fontsize=14)

# Custom percentage changes
percentage_labels = ['+ 2.49%', '- 5.72%']
for rect2, label in zip(bars2, percentage_labels):
    height = rect2.get_height()
    ax.text(rect2.get_x() + rect2.get_width() / 2.0, height, label, ha='center', va='bottom', fontsize=14)

plt.savefig("memory.pdf", format='pdf', dpi=2000, bbox_inches='tight')
plt.savefig("memory.png", format='png', dpi=2000, bbox_inches='tight')
# Display the plot
plt.show()
