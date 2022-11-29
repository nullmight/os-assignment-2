import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys

NUM_ITER = 3

n, m, m2 = sys.argv[1:4]

for tq in range(1, 3):
    P1_list = []
    for i in range(1, NUM_ITER + 1):
        P1_data = pd.read_csv(f"P1_{tq}_{i}.csv")
        P1_data = np.array(P1_data.groupby('P1')[['P1', 'Time']].min())
        P1_list.append(P1_data)
        # plt.plot(P1_data)
        # plt.savefig(f"P1_{tq}_{i}", bbox_inches='tight')
        # plt.show()

    color1 = '#2E86E0'
    P1_med = np.median(np.dstack(P1_list), -1)
    plt.plot(P1_med[:, 0], P1_med[:, 1], marker='.', linestyle = '--', color=color1)
    plt.title(
        f"P1, Time Quantum {tq}ms, {n}x{m} multiplied by {m}x{m2}", fontweight='bold')
    plt.xlabel('Number of threads in P1', fontweight='bold', color=color1)
    plt.ylabel('Time taken(in ns)', fontweight='bold', color=color1)
    min_idx = np.argmin(P1_med[:, 1])
    nt, time = P1_med[min_idx, 0], P1_med[min_idx, 1]
    plt.scatter(nt, time, color='black')
    plt.annotate(f"{nt}, {time}", (nt, time), fontweight='bold')
    plt.savefig(f"P1_{n}_{m}_{m2}_{tq}.png", bbox_inches='tight')
    plt.show()

    P2_list = []
    for i in range(1, NUM_ITER + 1):
        P2_data = pd.read_csv(f"P2_{tq}_{i}.csv")
        P2_data = np.array(P2_data.groupby('P2')[['P2', 'Time']].min())
        P2_list.append(P2_data)
        # plt.plot(P2_data)
        # plt.savefig(f"P2_{tq}_{i}", bbox_inches='tight')
        # plt.show()

    color2 = '#D43790'
    P2_med = np.median(np.dstack(P2_list), -1)
    plt.plot(P2_med[:, 0], P2_med[:, 1], marker='.', linestyle = '--', color=color2)
    plt.title(f"P2, Time Quantum {tq}ms, {n}x{m} multiplied by {m}x{m2}", fontweight='bold')
    plt.xlabel('Number of threads in P2', fontweight='bold', color=color2)
    plt.ylabel('Time taken(in ns)', fontweight='bold', color=color2)
    min_idx = np.argmin(P2_med[:, 1])
    nt, time = P2_med[min_idx, 0], P2_med[min_idx, 1]
    plt.scatter(nt, time, color='black')
    plt.annotate(f"{nt}, {time}", (nt, time), fontweight='bold')
    plt.savefig(f"P2_{n}_{m}_{m2}_{tq}.png", bbox_inches='tight')
    plt.show()
