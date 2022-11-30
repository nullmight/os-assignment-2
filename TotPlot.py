import matplotlib.pyplot as plt
import pandas as pd
import numpy as np


TA_P1_1 = pd.read_csv("TA_P1_1.csv")
TA_P1_1 = np.array(TA_P1_1.groupby('Size')[['Size', 'Time']].mean())
plt.plot(TA_P1_1[:, 0], TA_P1_1[:, 1], marker='.', label='Time Quantum 1ms')

TA_P1_2 = pd.read_csv("TA_P1_2.csv")
TA_P1_2 = np.array(TA_P1_2.groupby('Size')[['Size', 'Time']].mean())
plt.plot(TA_P1_2[:, 0], TA_P1_2[:, 1], marker='.', label='Time Quantum 2ms')

plt.title(f"P1, Turnaround Time vs Workload Size", fontweight='bold')
plt.xlabel('Workload Size(dimensions of matrices)')
plt.ylabel('Turnaround Time(in ns)')
plt.legend()
plt.savefig(f"TA_P1.png", bbox_inches='tight')
plt.show()



TA_P2_1 = pd.read_csv("TA_P2_1.csv")
TA_P2_1 = np.array(TA_P2_1.groupby('Size')[['Size', 'Time']].mean())
plt.plot(TA_P2_1[:, 0], TA_P2_1[:, 1], marker='.', label='Time Quantum 1ms')

TA_P2_2 = pd.read_csv("TA_P2_2.csv")
TA_P2_2 = np.array(TA_P2_2.groupby('Size')[['Size', 'Time']].mean())
plt.plot(TA_P2_2[:, 0], TA_P2_2[:, 1], marker='.', label='Time Quantum 2ms')

plt.title(f"P2, Turnaround Time vs Workload Size", fontweight='bold')
plt.xlabel('Workload Size(dimensions of matrices)')
plt.ylabel('Turnaround Time(in ns)')
plt.legend()
plt.savefig(f"TA_P2.png", bbox_inches='tight')
plt.show()


WT_P1_1 = pd.read_csv("WT_1.csv")
WT_P1_1 = np.array(WT_P1_1.groupby('Size')[['Size', 'P1']].mean())
plt.plot(WT_P1_1[:, 0], WT_P1_1[:, 1], marker='.', label='Time Quantum 1ms')

WT_P1_2 = pd.read_csv("WT_2.csv")
WT_P1_2 = np.array(WT_P1_2.groupby('Size')[['Size', 'P1']].mean())
plt.plot(WT_P1_2[:, 0], WT_P1_2[:, 1], marker='.', label='Time Quantum 2ms')

plt.title(f"P1, Waiting Time vs Workload Size", fontweight='bold')
plt.xlabel('Workload Size(dimensions of matrices)')
plt.ylabel('Waiting Time(in ns)')
plt.legend()
plt.savefig(f"WT_P1.png", bbox_inches='tight')
plt.show()


WT_P2_1 = pd.read_csv("WT_1.csv")
WT_P2_1 = np.array(WT_P2_1.groupby('Size')[['Size', 'P2']].mean())
plt.plot(WT_P2_1[:, 0], WT_P2_1[:, 1], marker='.', label='Time Quantum 1ms')

WT_P2_2 = pd.read_csv("WT_2.csv")
WT_P2_2 = np.array(WT_P2_2.groupby('Size')[['Size', 'P2']].mean())
plt.plot(WT_P2_2[:, 0], WT_P2_2[:, 1], marker='.', label='Time Quantum 2ms')

plt.title(f"P2, Waiting Time vs Workload Size", fontweight='bold')
plt.xlabel('Workload Size(dimensions of matrices)')
plt.ylabel('Waiting Time(in ns)')
plt.legend()
plt.savefig(f"WT_P2.png", bbox_inches='tight')
plt.show()



WT_P1_1 = pd.read_csv("WT_1.csv")
WT_P1_1 = np.array(WT_P1_1.groupby('Size')[['Size', 'P1']].mean())
plt.plot(WT_P1_1[:, 0], WT_P1_1[:, 1], marker='.', label='Time Quantum 1ms')

WT_P1_2 = pd.read_csv("WT_2.csv")
WT_P1_2 = np.array(WT_P1_2.groupby('Size')[['Size', 'P1']].mean())
plt.plot(WT_P1_2[:, 0], WT_P1_2[:, 1], marker='.', label='Time Quantum 2ms')

plt.title(f"P1, Waiting Time vs Workload Size", fontweight='bold')
plt.xlabel('Workload Size(dimensions of matrices)')
plt.ylabel('Waiting Time(in ns)')
plt.legend()
plt.savefig(f"WT_P1.png", bbox_inches='tight')
plt.show()


WT_P2_1 = pd.read_csv("WT_1.csv")
WT_P2_1 = np.array(WT_P2_1.groupby('Size')[['Size', 'P2']].mean())
plt.plot(WT_P2_1[:, 0], WT_P2_1[:, 1], marker='.', label='Time Quantum 1ms')

WT_P2_2 = pd.read_csv("WT_2.csv")
WT_P2_2 = np.array(WT_P2_2.groupby('Size')[['Size', 'P2']].mean())
plt.plot(WT_P2_2[:, 0], WT_P2_2[:, 1], marker='.', label='Time Quantum 2ms')

plt.title(f"P2, Waiting Time vs Workload Size", fontweight='bold')
plt.xlabel('Workload Size(dimensions of matrices)')
plt.ylabel('Waiting Time(in ns)')
plt.legend()
plt.savefig(f"WT_P2.png", bbox_inches='tight')
plt.show()


CS_1 = pd.read_csv("CS_1.csv").to_numpy()
plt.plot(CS_1[:, 0], CS_1[:, 1], marker='.', label='Time Quantum 1ms')

CS_2 = pd.read_csv("CS_2.csv").to_numpy()
plt.plot(CS_2[:, 0], CS_2[:, 1], marker='.', label='Time Quantum 2ms')

plt.title(f"Context Switching Time vs Workload Size", fontweight='bold')
plt.xlabel('Workload Size(dimensions of matrices)')
plt.ylabel('Context Switching Time(in ns)')
plt.legend()
plt.savefig(f"CS.png", bbox_inches='tight')
plt.show()
