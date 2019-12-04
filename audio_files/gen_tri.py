import numpy as np
from scipy import signal

base_freq = 1000
# High Freq sin
x = np.linspace(0, 1, base_freq)
s = np.array([i * 32767 for i in signal.sawtooth(2 * np.pi * x)] * (base_freq // 10)).astype(np.int16)

# Low Freq sin
y = np.linspace(0, 1, base_freq * 2)
t = np.array([i * 32767 for i in signal.sawtooth(2 * np.pi * x)] * (base_freq // 20)).astype(np.int16)

print("s: {}".format(len(s)))
print("t: {}".format(len(t)))
f = open('tri.txt', 'wb')
for x in s:
    f.write(x)
f.close()
