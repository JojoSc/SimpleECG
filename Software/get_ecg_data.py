import serial
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import (AutoMinorLocator, MultipleLocator)
from scipy import signal
import numpy as np
from time import sleep
import os
import serial.tools.list_ports
import struct

script_dir = os.path.dirname(os.path.abspath(__file__))
#csv_path = os.path.join(script_dir, 'df.csv')

sleep(0)

# Set up serial connection
#ser = serial.Serial('/dev/tty.usbmodem8D81277A50571')
ports = serial.tools.list_ports.comports(include_links=False)
ecg_port = ""
for port in ports :
    if "usbmodem" in port.device:
        ecg_port = port.device

ser = serial.Serial(ecg_port)
sampling_rate = 1000


# Create empty dataframe to hold data
df = pd.DataFrame(columns=['III', 'I', 'V1', 'V2', 'V3', 'V4', 'V5', 'V6'])

# Record and plot data
t = 0
while len(df) < 12000:
    # Read line from serial and split into values
    # line = ser.readline().decode().strip()
    # values = line.split(',')
    # row = {'Value 1': int(values[0]), 'Value 2': int(values[1]), 'Value 3': int(values[2]), 'Value 4': int(values[3]), 'Value 5': int(values[4]), 'Value 6': int(values[5]), 'Value 7': int(values[6]), 'Value 8': int(values[7])}
    
    row = {}
    data = ser.read(40)
    if len(data) == 40 and data[-4:] == b'\xFF\xFF\xFF\xFF':
        uvalues = struct.unpack('<10i', data)
        values = [signed_int | ~0xFFFFFF if signed_int & 0x800000 else signed_int for signed_int in uvalues]

        row = {'III': int(values[0]), 'I': -int(values[1]), 'V1': int(values[2]), 'V2': int(values[3]), 'V3': int(values[4]), 'V4': int(values[5]), 'V5': int(values[6]), 'V6': int(values[7]), 'time': t}
        t += 1
        print(t)
    else:
        print(data)
        print()
    # except Exception as e:
    #     print(e)
    
    df = df.append(row, ignore_index=True)


df = df.iloc[10:-10]
df.to_csv(script_dir + "/ecg2.csv")

# df = pd.read_csv(csv_path)
# df.index = np.arange(0, len(df), 1)
# df.index = df.index / sampling_rate

fig, axes = plt.subplots(8, 1)

for i in range(1, 9):
    df['Value ' + str(i) + " filtered"] = df["Value " + str(i)]



    df.fillna(method="bfill", inplace=True)
    df['Value ' + str(i) + " filtered"] = 2*2.4 * df['Value ' + str(i) + " filtered"] / 6 / (2**23 - 1) * 1000
    df['Value ' + str(i) + " filtered"] = df['Value ' + str(i) + " filtered"].rolling(window=10).mean()
    df.fillna(method="bfill", inplace=True)


    # Create the bandstop filter
    #b, a = signal.butter(1, [45, 55], btype="bandstop", fs=sampling_rate)
    b, a = signal.iirnotch(50, 1, sampling_rate)
    #b3, a3 = signal.iirnotch(100, 1, sampling_rate)
    # b5, a5 = signal.iirnotch(150, 0.5, sampling_rate)
    # b6, a6 = signal.iirnotch(250, 0.5, sampling_rate)
    b2, a2 = signal.butter(1, 150, btype="lowpass", fs=sampling_rate)
    b4, a4 = signal.butter(1, 0.05, btype="highpass", fs=sampling_rate)

    # Apply the filter to the signal column of the DataFrame
    df['Value ' + str(i) + " filtered"] = signal.filtfilt(b, a, df['Value ' + str(i) + " filtered"]) # 50hz
    # df['Value ' + str(i) + " filtered"] = signal.filtfilt(b2, a2, df['Value ' + str(i) + " filtered"]) # 100hz lowpass
    # df['Value ' + str(i) + " filtered"] = signal.filtfilt(b5, a5, df['Value ' + str(i) + " filtered"]) # 150hz
    #df['Value ' + str(i) + " filtered"] = signal.filtfilt(b4, a4, df['Value ' + str(i) + " filtered"]) # highpass
    ####df['B'] = signal.filtfilt(b6, a6, df['B']) # 250 hz

    df.fillna(method="bfill", inplace=True)

    ax = axes[i-1]
    ax.plot(df['Value ' + str(i) + " filtered"])
    #ax.set_ylim([-2, 2])
    ax.xaxis.set_major_locator(MultipleLocator(1000))
    ax.yaxis.set_major_locator(MultipleLocator(1))
    ax.xaxis.set_minor_locator(AutoMinorLocator(10))
    ax.yaxis.set_minor_locator(AutoMinorLocator(10))
    ax.grid(which='major', color='#CCCCCC', linestyle='--')
    ax.grid(which='minor', color='#CCCCCC', linestyle=':')
    #ax.set_ylabel()

plt.ylabel("mV")
plt.legend()
plt.show()


##############################

# Extract the column you want to analyze
column = df['I']


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