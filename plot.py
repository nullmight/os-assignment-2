import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys

NUM_ITER = 3

n, m, m2 = sys.argv[1:4]

for tq in range(1, 2):
    P1_list = []
    for i in range(1, NUM_ITER):
        P1_data = pd.read_csv(f"P1_{tq}_{i}.csv")
        P1_data = np.array(P1_data.groupby('P1')[['P1', 'Time']].min())
        P1_list.append(P1_data)

    color1 = '#2E86E0'
    P1_med = np.median(np.dstack(P1_list), -1)
    plt.plot(P1_med[:, 0], P1_med[:, 1], marker='.', linestyle = '--', color=color1)
    plt.title(
        f"P1, time_quantum={tq}, {n}x{m} multiplied by {m}x{m2}", fontweight='bold')
    plt.xlabel('Number of threads in P1', fontweight='bold', color=color1)
    plt.ylabel('Time taken(in ns)', fontweight='bold', color=color1)
    plt.savefig(f"P1_{sys.argv[1]}_{sys.argv[2]}_{sys.argv[3]}.png", bbox_inches='tight')
    plt.annotate('local min', xy=(0, 0),  xycoords='data',
                    xytext=(69, 420), textcoords='axes fraction',
                    arrowprops=dict(facecolor='black', shrink=0.05),
                    horizontalalignment='right', verticalalignment='top',
                    )
    plt.show()

    P2_list = []
    for i in range(1, NUM_ITER):
        P2_data = pd.read_csv(f"P2_{tq}_{i}.csv")
        P2_data = np.array(P2_data.groupby('P2')[['P2', 'Time']].min())
        P2_list.append(P2_data)

    color2 = '#D43790'
    P2_med = np.median(np.dstack(P2_list), -1)
    plt.plot(P2_med[:, 0], P2_med[:, 1], marker='.', linestyle = '--', color=color2)
    plt.title(f"P2, time_quantum={tq} {n}x{m} multiplied by {m}x{m2}", fontweight='bold')
    plt.xlabel('Number of threads in P2', fontweight='bold', color=color2)
    plt.ylabel('Time taken(in ns)', fontweight='bold', color=color2)
    plt.savefig(f"P2_{sys.argv[1]}_{sys.argv[2]}_{sys.argv[3]}.png", bbox_inches='tight')
    plt.show()
