import serial
import pandas as pd
import matplotlib.pyplot as plt
from scipy import signal
import numpy as np
from time import sleep
import os

script_dir = os.path.dirname(os.path.abspath(__file__))
csv_path = os.path.join(script_dir, 'df.csv')

sleep(0)

# Set up serial connection
ser = serial.Serial('/dev/tty.usbmodem8D81277A50571')

sampling_rate = 1000


# Create empty dataframe to hold data
df = pd.DataFrame(columns=['Value 1', 'Value 2', 'Value 3', 'Value 4'])

# Record and plot data
# while len(df) < 20000:
#     # Read line from serial and split into values
#     line = ser.readline().decode().strip()
#     values = line.split(',')
#     row = {'Value 1': int(values[0]), 'Value 2': int(values[1]), 'Value 3': int(values[2]), 'Value 4': int(values[3])}
#     df = df.append(row, ignore_index=True)

df = pd.read_csv(csv_path)

#df["Value 1"].plot()
#df["Value 2"].plot()

df['B'] = df['Value 1']



df.fillna(method="bfill", inplace=True)
df['B'] = 2*2.4 * df['B'] / 6 / (2**24 - 1) * 1000
df['B'] = df['B'].rolling(window=10).mean()
df.fillna(method="bfill", inplace=True)


# Create the bandstop filter
#b, a = signal.butter(1, [45, 55], btype="bandstop", fs=sampling_rate)
b, a = signal.iirnotch(50, 0.6, sampling_rate)
#b3, a3 = signal.iirnotch(100, 1, sampling_rate)
b5, a5 = signal.iirnotch(150, 0.5, sampling_rate)
b6, a6 = signal.iirnotch(250, 0.5, sampling_rate)
b2, a2 = signal.butter(1, 100, btype="lowpass", fs=sampling_rate)
b4, a4 = signal.butter(1, 0.05, btype="highpass", fs=sampling_rate)

# Apply the filter to the signal column of the DataFrame
df['B'] = signal.filtfilt(b, a, df['B']) # 50hz
df['B'] = signal.filtfilt(b2, a2, df['B']) # 100hz lowpass
df['B'] = signal.filtfilt(b5, a5, df['B']) # 150hz
#df['B'] = signal.filtfilt(b6, a6, df['B']) # 250 hz
df['xxx'] = df['B']

df.fillna(method="bfill", inplace=True)





df.fillna(method="bfill", inplace=True)




#df['A'].plot()
df['B'].plot(linewidth=1)



print(df.head())
print(df.tail())

plt.ylabel("mV")
plt.show()


##############################

# Extract the column you want to analyze
column = df['xxx']


# Get the length of the signal
signal_length = len(column)

# Calculate the frequency resolution
frequency_resolution = sampling_rate / signal_length

# Calculate the FFT of the signal
fft = np.fft.fft(column)

# Get the amplitude of the FFT (absolute value)
amplitude_spectrum = np.abs(fft)

# Get the frequency values for the x-axis of the plot
frequencies = np.arange(signal_length) * frequency_resolution

# Plot the frequency spectrum
plt.plot(frequencies, amplitude_spectrum)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude')

plt.show()