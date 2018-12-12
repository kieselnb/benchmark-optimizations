import numpy as np
import math
import matplotlib.pyplot as plt
fig = plt.figure()
ax = fig.add_subplot(111)

y_baseline = np.array([5165, 8114, 16851, 26926, 51358, 95591]);
y_quick = np.array([207, 473, 959, 2317, 4881, 10694]);
y_fast = np.array([904, 1072, 1523, 2106, 4098, 6229]);
data = np.array([8, 16, 32, 64, 128, 256]);

y_baseline_thrpt = [];
y_quick_thrpt = [];
y_less_thrpt = [];
y_fast_thrpt = [];

for i, j in zip(data, y_baseline):
    y_baseline_thrpt.append((i * 30 / j) * 2.4);

for i, j in zip(data, y_quick):
    y_quick_thrpt.append((i * math.log(i) / j) * 2.4);

for i, j in zip(data, y_fast):
    y_fast_thrpt.append((i * 19 / j) * 2.4);

y_peak = np.array([2.4, 2.4, 2.4, 2.4, 2.4, 2.4]);

plt.plot(data, y_baseline_thrpt, 'o-', label='Baseline plot')
plt.plot(data, y_quick_thrpt, "r-", label='Library implementation of sorting')
plt.plot(data, y_fast_thrpt, label='Current fastest version')
plt.plot(data, y_peak, label='Expected peak performance')
plt.legend(loc = 'lower right')
plt.xlabel("input size (number of integer keys")
plt.ylabel("Current GFLOPS/s for single core single thread performance")
plt.grid()
plt.show()
