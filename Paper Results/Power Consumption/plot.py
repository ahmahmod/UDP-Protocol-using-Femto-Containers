import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from oml_plot_tools import consum

def plot_comparison(df1_f, df1_g, df2_f, df2_g, df3_f, df3_g, title, labels):
    # Extract power data and convert to mW
    power1_f = df1_f['power'] * 1000
    power1_g = df1_g['power'] * 1000
    power2_f = df2_f['power'] * 1000
    power2_g = df2_g['power'] * 1000
    power3_f = df3_f['power'] * 1000
    power3_g = df3_g['power'] * 1000

    # Create DataFrames for ease of computation
    data = {
        labels[0]: power1_f,
        labels[1]: power1_g,
        labels[2]: power2_f,
        labels[3]: power2_g,
        labels[4]: power3_f,
        labels[5]: power3_g
    }

    df = pd.DataFrame(data)

    # Calculate means and standard deviations
    means = df.mean()
    stds = df.std()

    # Calculate absolute and percentage differences
    abs_diff_1 = abs(means[labels[0]] - means[labels[1]])
    perc_diff_1 = (abs_diff_1 / means[labels[0]]) * 100

    abs_diff_2 = abs(means[labels[2]] - means[labels[3]])
    perc_diff_2 = (abs_diff_2 / means[labels[2]]) * 100

    abs_diff_3 = abs(means[labels[4]] - means[labels[5]])
    perc_diff_3 = (abs_diff_3 / means[labels[4]]) * 100

    # Print mean, standard deviation, absolute difference, and percentage difference for each dataset
    for label in labels:
        print(f"{label} - Average Power: {means[label]:.5f} mWatts, Std Dev: {stds[label]:.5f} mWatts")
    
    print(f"\n{labels[0]} vs {labels[1]} - Absolute Difference: {abs_diff_1:.5f} mWatts, Percentage Difference: {perc_diff_1:.2f}%")
    print(f"{labels[2]} vs {labels[3]} - Absolute Difference: {abs_diff_2:.5f} mWatts, Percentage Difference: {perc_diff_2:.2f}%")
    print(f"{labels[4]} vs {labels[5]} - Absolute Difference: {abs_diff_3:.5f} mWatts, Percentage Difference: {perc_diff_3:.2f}%")

    # Set up the figure and axis
    fig, ax = plt.subplots(figsize=(10, 7))
    ax.set_title(title, fontsize=16)
    ax.set_ylabel('Power (mW)', fontsize=14)

    # Plot data with error bars
    x = np.arange(len(labels))

    for i, label in enumerate(labels):
        if 'GNRC' in label:
            ax.errorbar(x[i], means[label], yerr=stds[label], fmt='o', capsize=10, label=label, color='g')
            # Add a dotted vertical line after each GNRC label
            ax.axvline(x=i + 0.5, color='grey', linestyle=':', linewidth=1)
        else:
            ax.errorbar(x[i], means[label], yerr=stds[label], fmt='o', capsize=10, label=label, color='b')

    ax.set_xticks(x)
    ax.set_xticklabels(labels, fontsize=14, rotation=45)
    ax.yaxis.grid(True)
    #ax.legend(fontsize=14)

    # Show plot
    plt.tight_layout()
    plt.savefig("no_radio_power.pdf", format='pdf', dpi=2000, bbox_inches='tight')
    plt.savefig("no_radio_power.png", format='png', dpi=2000, bbox_inches='tight')
    plt.savefig("no_radio_power.svg", format='svg', dpi=2000, bbox_inches='tight')
    plt.show()

# Load data
data_1f = consum.oml_load('1_fc.oml')[1000:]
data_1g = consum.oml_load('1_gnrc.oml')[1000:]
data_2f = consum.oml_load('2_fc.oml')[1000:]
data_2g = consum.oml_load('2_gnrc.oml')[1000:]
data_3f = consum.oml_load('3_fc.oml')[1000:]
data_3g = consum.oml_load('3_gnrc.oml')[1000:]

# Convert to pandas DataFrames
df_1f = pd.DataFrame(data_1f)
df_1g = pd.DataFrame(data_1g)
df_2f = pd.DataFrame(data_2f)
df_2g = pd.DataFrame(data_2g)
df_3f = pd.DataFrame(data_3f)
df_3g = pd.DataFrame(data_3g)

# Plot comparison
plot_comparison(df_1f, df_1g, df_2f, df_2g, df_3f, df_3g,
                'Power Consumption Comparison',
                ['FC_1', 'GNRC_1', 'FC_2', 'GNRC_2', 'FC_3', 'GNRC_3'])
