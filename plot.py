import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

P1_list = []
P2_list = []
for i in range(1, 5):
    P1_data = pd.read_csv(f"P1_{i}.csv").to_numpy()
    P1_list.append(P1_data)
    # plt.plot(P1_data)
    # plt.savefig(f"P1_bench_1000_{i}.png", bbox_inches='tight')
    # plt.show()
    P2_data = pd.read_csv(f"P2_{i}.csv").to_numpy()
    P2_list.append(P2_data)

P1_med = np.median(np.dstack(P1_list), -1)
plt.plot(P1_med)
plt.savefig(f"P1_5_5_5.png", bbox_inches='tight')
plt.show()