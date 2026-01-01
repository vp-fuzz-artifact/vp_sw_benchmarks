# usage: python plot_edges.py mydata1.txt mydata2.txt [mode]
import pandas as pd
import matplotlib.pyplot as plt
import sys
import os
import numpy as np
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

    # Clean numeric-like columns
    for col in df.columns:
        if df[col].dtype == 'object':
            df[col] = df[col].astype(str).str.replace('%', '', regex=False)
            df[col] = pd.to_numeric(df[col], errors='ignore')

    return df


def align_times(x1, y1, x2, y2, mode="extend"):
    """Aligns two time series so both start at 0 and end at the same time."""
    x1 = x1 - x1.min()
    x2 = x2 - x2.min()
    t_max = max(x1.max(), x2.max())

    # Common time axis
    common_time = np.linspace(0, t_max, max(len(x1), len(x2)))

    # Interpolate both to common time
    y1_interp = np.interp(common_time, x1, y1)
    y2_interp = np.interp(common_time, x2, y2)

    if mode == "cut":
        t_max_cut = min(x1.max(), x2.max())
        mask = common_time <= t_max_cut
        common_time = common_time[mask]
        y1_interp = y1_interp[mask]
        y2_interp = y2_interp[mask]
    elif mode != "extend":
        raise ValueError("mode must be 'extend' or 'cut'")

    return common_time, y1_interp, y2_interp


def plot_combined(file1, file2, mode="extend"):
    df1 = read_custom_csv(file1)
    df2 = read_custom_csv(file2)

    # === Extract ===
    x1 = pd.to_numeric(df1['relative_time'], errors='coerce')
    y1 = pd.to_numeric(df1['edges_found'], errors='coerce')
    df1 = df1.dropna(subset=['relative_time', 'edges_found']).sort_values('relative_time')

    x2 = pd.to_numeric(df2['unix_time'], errors='coerce')
    y2 = pd.to_numeric(df2['paths_total'], errors='coerce')
    df2 = df2.dropna(subset=['unix_time', 'paths_total']).sort_values('unix_time')

    # === Align both time series ===
    time, y1_aligned, y2_aligned = align_times(x1, y1, x2, y2, mode=mode)

    # === Plot ===
    plt.figure(figsize=(9, 5))

    # Line 1 — Edges Found
    plt.plot(time, y1_aligned, '-', color='tab:blue', linewidth=1.2, label='Edges Found')

    # Line 2 — Paths Total
    plt.plot(time, y2_aligned, '-', color='tab:orange', linewidth=1.2, label='Paths Total')

    # Downsampled markers
    max_markers = 40
    marker_idx = np.unique(np.round(np.linspace(0, len(time) - 1, min(max_markers, len(time)))).astype(int))
    plt.plot(time[marker_idx], y1_aligned[marker_idx], 'x', color='tab:blue', markersize=6, label='_nolegend_')
    plt.plot(time[marker_idx], y2_aligned[marker_idx], 'o', color='tab:orange', markersize=5, label='_nolegend_')

    # Labels and legend
    plt.title('Edges Found vs Paths Total Over Time', fontsize=13)
    plt.xlabel('Time (seconds)')
    plt.ylabel('Count')
    plt.grid(alpha=0.25)
    plt.legend(loc='best')
    plt.tight_layout()

    # Save image
    base1, _ = os.path.splitext(os.path.basename(file1))
    base2, _ = os.path.splitext(os.path.basename(file2))
    output_file = f"{base1}_vs_{base2}_{mode}.png"
    plt.savefig(output_file, dpi=200)
    print(f"Saved combined plot to {output_file}")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: python {sys.argv[0]} <file1> <file2> [mode]")
        sys.exit(1)

    mode = sys.argv[3] if len(sys.argv) > 3 else "extend"
    plot_combined(sys.argv[1], sys.argv[2], mode)
