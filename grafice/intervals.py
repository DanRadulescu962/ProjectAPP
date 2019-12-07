from ploting import *  # take the plotting functions
import numpy as np

# nr_of_elems = [1, 2, 4, 8, 16]

# labels = ["PThreads", "OpenMP", "MPI"]

pthrea = [79.438, 42.103, 31.007, 29.562, 26.521]
MPIrun = [68.159, 41.979, 27.012, 21.543, 14.204]
OpenMP = [84.167, 56.576, 42.504, 38.586, 32.088]

# plt.figure(figsize=(10, 6))  # you can insert a number for keeping an order of the figures
# plt.title("Comparison of execution time for range of 100 iterations for KMEANS")
# plt.xlabel("Number of workers(cpus/threads)")  # optional
# plt.ylabel("Elapsed Time (seconds)")  # optional
# # plt.plot(nr_of_elems, MPIrun, 'r-', linewidth=2, markersize=12)
# plt.plot(nr_of_elems, pthrea, 'b-', nr_of_elems, OpenMP, 'g-',  nr_of_elems, MPIrun, 'r-', linewidth=2, markersize=12)
# plt.legend(labels)
# plt.show()


labels = [1, 2, 4, 8, 16]

# OpenMP = [7.805, 6.557, 4.312, 3.359, 2.460, 1.927, 1.889, 1.742]
# MPIrun = [8.692, 7.931, 4.699, 4.239, 3.538, 3.009, 2.825, 2.810]
# hibrid = [7.336, 7.122, 4.279, 3.767, 3.177, 2.888, 2.678, 1.910]
# pthrea = [7.483, 6.392, 4.168, 3.270, 2.311, 1.561, 1.346, 1.108]
# labels =["OpenMP", "PThreads", "MPI", "Hybrid MPI-OpenMP"]
# labels =["OpenMP", "PThreads"]
x = np.arange(len(labels))  # the label locations
width = 0.2  # the width of the bars

# plt.figure(figsize=(10, 6))

fig, ax = plt.subplots(figsize=(10, 6))

rects1 = ax.bar(x - width, MPIrun, width, label='MPI')
rects2 = ax.bar(x, OpenMP, width, label='OpenMP')
rects3 = ax.bar(x + width, pthrea, width, label='PThreads')

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_ylabel("Elapsed Time (seconds)")
ax.set_xlabel("Number of workers(cpus/threads)")
ax.set_title('Comparison of execution time for 100 iterations of KMEANS')
ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.legend()


def autolabel(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax.annotate('{}'.format(height),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom')


autolabel(rects1)
autolabel(rects2)
autolabel(rects3)

fig.tight_layout()

plt.show()
