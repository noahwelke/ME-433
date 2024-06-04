import matplotlib.pyplot as plt
import numpy as np
import csv

t = [] # column 0
data1 = [] # column 1
data2 = [] # column 2

with open('sigB.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column
        #data2.append(float(row[2])) # third column

N = 1
m = len(t) - N
A = 0.97
new_average = np.zeros(m)

for i in range(1,m):
    new_average[i] = A * new_average[i-1] + (1-A) * data1[i]

tbox = t[0:m]

Fs = 10000 # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y = data1 # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

Fs = 10000 # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,tbox[-1],Ts) # time vector
y2 = new_average # the data to make the fft from
n = len(y2) # length of the signal
k = np.arange(n)
T = n/Fs
frq2 = k/T # two sides frequency range
frq2 = frq2[range(int(n/2))] # one side frequency range
Y2 = np.fft.fft(y2)/n # fft computing and normalization
Y2 = Y2[range(int(n/2))]

fig, (ax1, ax2) = plt.subplots(2, 1)
plt.title('A=%.2f' %A)
ax1.plot(t,y,'k')
ax1.plot(tbox,y2,'r')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'k')
ax2.loglog(frq2,abs(Y2),'r') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()