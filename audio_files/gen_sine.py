import numpy as np

base_freq = 1000
# High Freq sin
x = np.linspace(-np.pi, np.pi, base_freq + 1)
s = np.array([i * 32767 for i in np.sin(x)] * (base_freq // 10)).astype(np.int16)

# Low Freq sin
y = np.linspace(-np.pi, np.pi, base_freq * 2 + 1)
t = np.array([i * 32767 for i in np.sin(y)] * (base_freq // 20)).astype(np.int16)

# print("s: {}".format(s))
f = open('sine.txt', 'wb')
for x in s:
    f.write(x)
f.close()
