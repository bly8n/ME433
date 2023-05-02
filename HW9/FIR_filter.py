
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
h = [
    0.000000000000000000,
    -0.000005622290253718,
    -0.000015952613718074,
    -0.000019164857121407,
    0.000000000000000000,
    0.000061151997198953,
    0.000189005632131738,
    0.000413777450584962,
    0.000771052143228499,
    0.001300980553345611,
    0.002046732813480528,
    0.003052205201402669,
    0.004359063037605834,
    0.006003285535379580,
    0.008011453006166186,
    0.010397073738067305,
    0.013157280105781339,
    0.016270226109782456,
    0.019693489417184757,
    0.023363721007878532,
    0.027197698740929570,
    0.031094834376003570,
    0.034941065856108781,
    0.038613948413921587,
    0.041988650142707258,
    0.044944470273701805,
    0.047371440083511518,
    0.049176543178285452,
    0.050289106802944697,
    0.050664968287519985,
    0.050289106802944704,
    0.049176543178285452,
    0.047371440083511518,
    0.044944470273701798,
    0.041988650142707251,
    0.038613948413921600,
    0.034941065856108795,
    0.031094834376003567,
    0.027197698740929577,
    0.023363721007878549,
    0.019693489417184764,
    0.016270226109782463,
    0.013157280105781339,
    0.010397073738067312,
    0.008011453006166198,
    0.006003285535379582,
    0.004359063037605840,
    0.003052205201402673,
    0.002046732813480528,
    0.001300980553345611,
    0.000771052143228498,
    0.000413777450584962,
    0.000189005632131737,
    0.000061151997198953,
    0.000000000000000000,
    -0.000019164857121407,
    -0.000015952613718074,
    -0.000005622290253718,
    0.000000000000000000,
]
X = len(h)
for i,dp in enumerate(data1):
    if i < X:
        # not enough samples yet
        f.append(dp)
    else:
        # apply the FIR filter to the last X samples
        filtered_dp =0 
        for j in range (X):#np.sum([h[j]*data1[i-j] for j in range(X)])
            filtered_dp=filtered_dp+h[j]*data1[i-j]
        f.append(filtered_dp)
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
ax1.plot(t,y1,'k',t,y2,'r')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y1),'k',frq,abs(Y2),'r') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.suptitle('FIR_filter_sigD,sampling_rate:10000,cutoff_frequency:200,bandwidth:800,window_type:Blackman')
plt.show()
