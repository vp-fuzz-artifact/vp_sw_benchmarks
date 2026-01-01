import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os
from io import StringIO
import matplotlib as mpl
from matplotlib.lines import Line2D

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


def plot_combined(ax, files, mode="extend", title=None, labels=None, show_ylabel=False):
    """Plot one overlapping graph into an existing subplot (ax)."""
    colors = ['tab:blue', 'tab:orange', 'tab:green']
    line_styles = ['-', '--', ':'] 
    line_width = [1.5, 2.5, 3] 

    
    default_labels = ['No Checkpoint', 'Fork', 'CRIU']
    if labels is None:
        labels = default_labels

    time_arrays, value_arrays = [], []
    crash_points = []  # Stores (relative_time, edge_count, line_index)

    for i, file in enumerate(files):
        df = read_custom_csv(file)
        
        if 'relative_time' in df.columns:
            df['relative_time'] = pd.to_numeric(df['relative_time'], errors='coerce')
            df['edges_found'] = pd.to_numeric(df['edges_found'], errors='coerce')
            df = df.dropna(subset=['relative_time', 'edges_found']).sort_values('relative_time')
            x = df['relative_time'].to_numpy()
            y = df['edges_found'].to_numpy()
            time_col, val_col = 'relative_time', 'edges_found'
        elif 'unix_time' in df.columns and 'paths_total' in df.columns:
            df['unix_time'] = pd.to_numeric(df['unix_time'], errors='coerce')
            df['paths_total'] = pd.to_numeric(df['paths_total'], errors='coerce')
            df = df.dropna(subset=['unix_time', 'paths_total']).sort_values('unix_time')
            x = df['unix_time'].to_numpy()
            y = df['paths_total'].to_numpy()
            time_col, val_col = 'unix_time', 'paths_total'
        else:
            raise ValueError(f"Unrecognized file format for {file}")
            
        time_arrays.append(x)
        value_arrays.append(y)

        # Dynamic search for crash data columns
        crash_cols = [col for col in df.columns if 'crash' in col.lower()]
        if crash_cols:
            c_col = crash_cols[0]
            df[c_col] = pd.to_numeric(df[c_col], errors='coerce').fillna(0)
            # Locate the very first instance where crashes hit 1 or more
            crash_df = df[df[c_col] >= 1]
            if not crash_df.empty:
                first_crash = crash_df.iloc[0]
                c_time = pd.to_numeric(first_crash[time_col], errors='coerce')
                c_val = pd.to_numeric(first_crash[val_col], errors='coerce')
                c_time_rel = c_time - x.min()
                # Pass 'i' along so we know which color to map to the crash dot
                crash_points.append((c_time_rel, c_val, i))

    common_time, aligned_values = align_times(time_arrays, value_arrays, mode=mode)

    ax._custom_handles = []

    for i, y in enumerate(aligned_values):
        x_ds, y_ds = downsample(common_time, y)
        line_zo = 2 + i

        # Plot line only (Regular repetitive markers removed completely)
        ax.plot(x_ds, y_ds, line_styles[i], color=colors[i], 
                linewidth=line_width[i] - (i * 0.4), zorder=line_zo, label='_nolegend_')

        # Create clean line handles for the legend (No markers inside line labels)
        handle = Line2D([0], [0],
                        color=colors[i],
                        linestyle=line_styles[i],
                        linewidth=line_width[i],
                        label=labels[i])
        ax._custom_handles.append(handle)

    # === Plot Single Crash Dots Matching the Parent Line Color ===
    for cx, cy, line_idx in crash_points:
        ax.plot(cx, cy, marker='o', color=colors[line_idx], markersize=7, 
                markeredgecolor='black', markeredgewidth=0.7, zorder=15, label='_nolegend_')

    # Append Legend icon for "First Crash" using a neutral outline marker
    if crash_points:
        crash_handle = Line2D([0], [0], color='none', marker='o', linestyle='',
                              markersize=7, markeredgecolor='black', markeredgewidth=1.0, 
                              markerfacecolor='darkgray', label='First Crash')
        ax._custom_handles.append(crash_handle)

    ax.set_xlabel("Time (seconds)")
    if show_ylabel:
        ax.set_ylabel("Edges Count")
        
    ax.set_title(title or "")
    ax.grid(alpha=0.25)
    ax.margins(y=0.08)


def plot_multiple_combined_sets(sets_of_files, mode="extend", big_title="Password checker"):
    """Create a 1x3 figure with each subplot showing overlapping curves."""
    n = len(sets_of_files)
    fig, axes = plt.subplots(1, n, figsize=(4.5 * n, 3), sharey=False)

    if n == 1:
        axes = [axes]

    labels_for_subplots = ["Bare-metal", "Zephyr", "Linux"]

    for i, (ax, files) in enumerate(zip(axes, sets_of_files)):
        small_title = f"{labels_for_subplots[i]}"
        plot_combined(ax, files, mode=mode, title=small_title, show_ylabel=(i == 0))

    # === FIXED LAYOUT ADJUSTMENTS ===
    plt.subplots_adjust(top=0.80, bottom=0.15, wspace=0.3)
    fig.suptitle(big_title, fontsize=13, fontweight='bold', y=1.04)

    # 1. Enforce a strict order for the legend items
    desired_order = ["No Checkpoint", "Fork", "CRIU", "First Crash"]
    handle_dict = {}
    for ax in axes:
        if hasattr(ax, '_custom_handles'):
            for h in ax._custom_handles:
                handle_dict[h.get_label()] = h

    global_handles = [handle_dict[lbl] for lbl in desired_order if lbl in handle_dict]
    global_labels = [lbl for lbl in desired_order if lbl in handle_dict]

    # 2. Render the legend cleanly across 1 unified row
    fig.legend(global_handles, global_labels,
           loc='upper center',
           ncol=len(global_handles),  
           frameon=False,
           columnspacing=1.8,         
           bbox_to_anchor=(0.5, 0.97))

    plt.savefig(big_title + ".png", dpi=200, bbox_inches='tight')
    print(f"Saved combined plot to {big_title}.png")


if __name__ == "__main__":
    args = sys.argv[1:]
    if len(args) < 2:
        print("Usage: python plot_combined_edges.py <mode> <'fileset1'> <'fileset2'> ...")
        sys.exit(1)

    mode = args[0] if args[0] in ("extend", "cut") else "extend"
    sets_of_files = [a.split() for a in args[1:]]
    plot_multiple_combined_sets(sets_of_files, mode=mode)