import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

P1_list = []
for i in range(1, 8):
    P1_data = pd.read_csv(f"P1_bench_{i}.csv").to_numpy()
    P1_list.append(P1_data)
    plt.plot(P1_data)
    plt.savefig(f"P1_bench_1000_{i}.png", bbox_inches='tight')
    plt.show()

P1_med = np.median(np.dstack(P1_list), -1)
plt.plot(P1_med)
plt.savefig(f"P1_bench_1000_median.png", bbox_inches='tight')
plt.show()
