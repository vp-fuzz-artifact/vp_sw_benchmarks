import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

path = "./cov_file.txt"

# Read file line by line and extract the number after the space
nums = []
with open(path, "r") as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) == 2 and parts[1].isdigit():
            nums.append(int(parts[1]))

# Create a DataFrame
df = pd.DataFrame({"num_bbs_total": nums})

# Create an artificial x-axis as execution index
df["exec_index"] = range(1, len(df) + 1)

plt.xlim(left=0)  # x-axis from 0
plt.ylim(bottom=0)  # y-axis from 0

plt.figure(figsize=(8,5))
plt.plot(df["num_bbs_total"], label="Bitmap Coverage", color="blue")
plt.xlabel("Execution Index")
plt.ylabel("Coverage (#BBs)")
plt.title("Coverage Growth Over Executions")
plt.legend()
plt.grid(True)

max_bb = df["num_bbs_total"].max()
plt.yticks(np.linspace(0, max_bb, 10, dtype=int))  # 10 ticks

plt.savefig("coverage_growth.png", dpi=200, bbox_inches="tight")