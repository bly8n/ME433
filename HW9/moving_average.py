import matplotlib.pyplot as plt
import numpy as np

#t = np.arange(0.0, 1.0, dt) # 10s
# a constant plus 100Hz and 1000Hz
#s = 4.0 * np.sin(2 * np.pi * 100 * t) + 0.25 * np.sin(2 * np.pi * 1000 * t) + 25
import csv

t = [] # column 0
data1 = [] # column 1
#data2 = [] # column 2

with open('sigD.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column
        #data2.append(float(row[2])) # third column
f=[]
A=0.99
B=0.01
for dp in data1:
    if len(f)==0:
       f.append(0)
    else:
       f.append((f[-1]+dp)/2)
dt=t[1]-t[0]
Fs = 1/dt # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y1 = data1 # the data to make the fft from
y2=f
n = len(y1) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y1 = np.fft.fft(y1)/n # fft computing and normalization
Y1 = Y1[range(int(n/2))]
Y2 = np.fft.fft(y2)/n # fft computing and normalization
Y2 = Y2[range(int(n/2))]
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t,y1,'b',t,y2,'r')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y1),'b',frq,abs(Y2),'r') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.suptitle('moving average filter_sigD')
plt.show()

