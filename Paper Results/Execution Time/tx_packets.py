import pandas as pd
import matplotlib.pyplot as plt

# Function to extract execution times from a text file
def extract_times(file_path):
    times = []
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            # Extract the time from the line
            time = int(line.split(' ')[-2])
            times.append(time)
    return times

# Define the file paths
file_paths = [
    'tx_filtered_FCgnrc_time.txt',
    'tx_filtered_fc_time.txt',
    'tx_filtered_GNRCfc_time.txt',
    'tx_filtered_gnrc_time.txt'
]

# Extract the times from each file
FCgnrc_times = extract_times(file_paths[0])
fc_times = extract_times(file_paths[1])
GNRCfc_times = extract_times(file_paths[2])
gnrc_times = extract_times(file_paths[3])

# Create a DataFrame for better handling
df = pd.DataFrame({
    'FCgnrc': FCgnrc_times,
    'fc': fc_times,
    'GNRCfc': GNRCfc_times,
    'gnrc': gnrc_times
})

linewidth = 1.0
# Plot the execution times
plt.figure(figsize=(10, 6))
plt.plot(df['fc'], label='FC -> FC', linestyle='--', marker='s',linewidth=linewidth, color= 'blue')
plt.plot(df['FCgnrc'], label='FC -> GNRC', linestyle='-', marker='o', linewidth=linewidth, color= 'orange')
plt.plot(df['GNRCfc'], label='GNRC -> FC', linestyle='-.', marker='^',linewidth=linewidth, color= 'red')
plt.plot(df['gnrc'], label='GNRC -> GNRC', linestyle=':', marker='x',linewidth=linewidth, color= 'green')


plt.xlabel('Packet Number', fontsize=14)
plt.ylabel('Execution Time (us)', fontsize=14)
plt.title('UDP Transmission Execution Time', fontsize=16)
plt.legend(fontsize=14)
plt.grid(True)

plt.tight_layout()
plt.savefig('tx_packets.pdf', format='pdf', dpi=2000, bbox_inches='tight')
plt.savefig('tx_packets.png', format='png', dpi=2000, bbox_inches='tight')
plt.savefig('tx_packets.svg', format='svg', dpi=2000, bbox_inches='tight')

plt.show()
