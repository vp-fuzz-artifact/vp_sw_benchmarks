# usage:
#   python plot_edges.py file1.txt file2.txt [file3.txt] [mode]
# Example:
#   python plot_edges.py f1.txt f2.txt extend
#   python plot_edges.py f1.txt f2.txt f3.txt cut

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os
from io import StringIO


def read_custom_csv(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()

    header_line = None
    for i, line in enumerate(lines):
        if line.strip().startswith('#'):
            header_line = line.strip().lstrip('#').strip()
            data_start = i + 1
            break
    if header_line is None:
        raise ValueError(f"No header line starting with '#' found in {filename}")

    columns = [c.strip() for c in header_line.split(',')]
    data_str = ''.join(lines[data_start:])
    df = pd.read_csv(StringIO(data_str), names=columns)
    df.columns = df.columns.str.strip()

    for col in df.columns:
        if df[col].dtype == 'object':
            df[col] = df[col].astype(str).str.replace('%', '', regex=False)
            df[col] = pd.to_numeric(df[col], errors='ignore')

    return df


def align_times(time_arrays, value_arrays, mode="extend"):
    """Align multiple time series so they start at 0 and end at same time."""
    # Normalize all start times
    time_arrays = [x - x.min() for x in time_arrays]
    t_max = max(x.max() for x in time_arrays)
    common_time = np.linspace(0, t_max, max(len(x) for x in time_arrays))

    aligned_values = [np.interp(common_time, x, y) for x, y in zip(time_arrays, value_arrays)]

    if mode == "cut":
        t_max_cut = min(x.max() for x in time_arrays)
        mask = common_time <= t_max_cut
        common_time = common_time[mask]
        aligned_values = [y[mask] for y in aligned_values]
    elif mode != "extend":
        raise ValueError("mode must be 'extend' or 'cut'")

    return common_time, aligned_values


def plot_combined(files, mode="extend"):
    # Colors and markers for up to 3 datasets
    colors = ['tab:blue', 'tab:orange', 'tab:green']
    line_styles = ['-', '-', '--']
    marker_styles = ['x', 'o', 's']  # cross, dot, square
    labels = ['No Checkpoint', 'Fork', 'CRIU']

    time_arrays, value_arrays = [], []

    for i, file in enumerate(files):
        df = read_custom_csv(file)

        if 'relative_time' in df.columns:
            x = pd.to_numeric(df['relative_time'], errors='coerce')
            y = pd.to_numeric(df['edges_found'], errors='coerce')
            df = df.dropna(subset=['relative_time', 'edges_found']).sort_values('relative_time')
        elif 'unix_time' in df.columns and 'paths_total' in df.columns:
            x = pd.to_numeric(df['unix_time'], errors='coerce')
            y = pd.to_numeric(df['paths_total'], errors='coerce')
            df = df.dropna(subset=['unix_time', 'paths_total']).sort_values('unix_time')
        else:
            raise ValueError(f"Unrecognized file format for {file}")

        time_arrays.append(x)
        value_arrays.append(y)

    # Align all datasets
    common_time, aligned_values = align_times(time_arrays, value_arrays, mode=mode)

    # === Plot ===
    plt.figure(figsize=(9, 5))
    max_markers = 40
    marker_idx = np.unique(np.round(np.linspace(0, len(common_time) - 1, min(max_markers, len(common_time)))).astype(int))

    for i, y in enumerate(aligned_values):
        plt.plot(common_time, y, line_styles[i], color=colors[i],
                 linewidth=1.2, label=labels[i])
        plt.plot(common_time[marker_idx], y[marker_idx],
                 linestyle='', marker=marker_styles[i],
                 markersize=6, color=colors[i], label='_nolegend_')

    plt.title('Edges Found', fontsize=13)
    plt.xlabel('Time (seconds)')
    plt.ylabel('Count')
    plt.grid(alpha=0.25)
    plt.legend(loc='best')
    plt.tight_layout()

    # === Save ===
    base_names = [os.path.splitext(os.path.basename(f))[0] for f in files]
    output_file = "_vs_".join(base_names) + f"_{mode}.png"
    plt.savefig(output_file, dpi=200)
    print(f"Saved combined plot to {output_file}")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: python {sys.argv[0]} <file1> <file2> [file3] [mode]")
        sys.exit(1)

    # detect mode
    args = sys.argv[1:]
    if args[-1] in ("extend", "cut"):
        mode = args[-1]
        files = args[:-1]
    else:
        mode = "extend"
        files = args

    plot_combined(files, mode)
