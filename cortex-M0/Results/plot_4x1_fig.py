import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os
from io import StringIO
import matplotlib as mpl
from matplotlib.lines import Line2D

# To execute this we acivated the venv of fuzzware :/
# source /home/chiara/.virtualenvs/fuzzware/bin/activate

# === Global style ===
mpl.rcParams.update({
    "text.usetex": False,                 # don't use external LaTeX
    "font.family": "serif",
    "font.serif": ["DejaVu Serif"],       # built-in serif similar to Latin Modern
    "axes.labelsize": 11,
    "axes.titlesize": 12,
    "legend.fontsize": 10,
    "xtick.labelsize": 10,
    "ytick.labelsize": 10,
})


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


def downsample(x, y, max_points=1000):
    """Reduce number of points for cleaner plotting."""
    if len(x) <= max_points:
        return x, y
    idx = np.linspace(0, len(x) - 1, max_points).astype(int)
    return x[idx], y[idx]


def plot_combined(ax, files, mode="extend", title=None, labels=None):
    """Plot one overlapping graph into an existing subplot (ax)."""
    colors = ['tab:blue', 'tab:orange', 'tab:green']
    line_styles = ['-', '-', '-']
    marker_styles = ['x', 'o', 's']
    default_labels = ['Our work', 'Fuzzware', 'P2IM']
    if labels is None:
        labels = default_labels

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

    common_time, aligned_values = align_times(time_arrays, value_arrays, mode=mode)
    max_markers = 25
    marker_idx = np.unique(np.round(np.linspace(
        0, len(common_time) - 1,
        min(max_markers, len(common_time))
    )).astype(int))

    # store custom handles in the Axes object (for the global legend)
    ax._custom_handles = []

    for i, y in enumerate(aligned_values):
        x_ds, y_ds = downsample(common_time, y)

        # Line (dense)
        ax.plot(x_ds, y_ds, line_styles[i], color=colors[i], linewidth=1.2, label='_nolegend_')

        # Sparse markers
        ax.plot(common_time[marker_idx], y[marker_idx], linestyle='',
                marker=marker_styles[i], markersize=5, color=colors[i], label='_nolegend_')

        # Create a handle that shows both line and marker for the global legend
        handle = Line2D([0], [0],
                        color=colors[i],
                        linestyle=line_styles[i],
                        marker=marker_styles[i],
                        markersize=5,
                        linewidth=1.2,
                        label=labels[i])
        ax._custom_handles.append(handle)

    ax.set_xlabel("Time (seconds)")
    ax.set_ylabel("Edges Count")
    ax.set_title(title or "")
    ax.grid(alpha=0.25)


def plot_multiple_combined_sets(sets_of_files, mode="extend", big_title="Coverage Comparison"):
    """Create a 1x4 figure with each subplot showing overlapping curves."""
    n = len(sets_of_files)
    fig, axes = plt.subplots(1, n, figsize=(4.5 * n, 4), sharey=True)

    if n == 1:
        axes = [axes]

    labels_for_subplots = ["A - Drone", "B - Robot", "C - Passthrough", "D - Babbling"]

    for i, (ax, files) in enumerate(zip(axes, sets_of_files)):
        small_title = f"{labels_for_subplots[i]}"
        plot_combined(ax, files, mode=mode, title=small_title)

    # === Global title ===
    #fig.suptitle(big_title, fontsize=15, y=1.02)

    # === Shared legend BELOW the title ===
    handles = axes[0]._custom_handles  # use our custom handles
    labels = [h.get_label() for h in handles]
    fig.legend(handles, labels,
           loc='upper center',
           ncol=3,
           frameon=False,
           bbox_to_anchor=(0.5, 0.97))

    # === Layout adjustment ===
    plt.subplots_adjust(top=0.85, bottom=0.12, wspace=0.3)
    plt.savefig("combined_4x_plot.svg", format="svg", bbox_inches='tight')
    print("Saved combined 4-panel plot to combined_4x_plot.svg")
    #plt.savefig("combined_4x_plot.png", dpi=200, bbox_inches='tight')
    #print("Saved combined 4-panel plot to combined_4x_plot.png")


if __name__ == "__main__":
    # Example of how to call it:
    #
    # python3 plot_combined_edges.py extend \
    #   "./usima/plot_data_drone.txt ./fuzzware/plot_data_drone.txt ./p2im/plot_data_drone.txt" \
    #   "./usima/plot_data_robot_new.txt ./fuzzware/plot_data_robot.txt ./p2im/plot_data_robot.txt" \
    #   "./usima/plot_data_uart.txt ./fuzzware/plot_data_uart.txt" \
    #   "./usima/plot_data_buggy_can.txt ./fuzzware/plot_data_buggy_can.txt"
    #
    args = sys.argv[1:]
    if len(args) < 2:
        print("Usage: python plot_combined_edges.py <mode> <'fileset1'> <'fileset2'> ...")
        sys.exit(1)

    mode = args[0] if args[0] in ("extend", "cut") else "extend"
    sets_of_files = [a.split() for a in args[1:]]
    plot_multiple_combined_sets(sets_of_files, mode=mode)
