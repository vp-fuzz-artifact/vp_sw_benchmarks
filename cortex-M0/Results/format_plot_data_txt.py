# read input file and produce formatted output

input_file = "/home/chiara/usima-testing/benchmark/cortex-M0/Results/usima/plot_data_robot.txt"   # change to your file path
output_file = "/home/chiara/usima-testing/benchmark/cortex-M0/Results/usima/plot_data_robot_new.txt" # or any name you want

# read the file
with open(input_file, "r") as f:
    lines = f.readlines()

# prepare output lines
header = "# relative_time, cycles_done, cur_item, corpus_count, pending_total, pending_favs, map_size, saved_crashes, saved_hangs, max_depth, execs_per_sec, total_execs, edges_found\n"
output_lines = [header]

# process each line
for i, line in enumerate(lines, start=1):
    parts = line.strip().split()
    if len(parts) == 2 and parts[0] == "num_bbs_total":
        edges_found = parts[1]
        # construct output line
        formatted_line = f"{i}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {edges_found}\n"
        output_lines.append(formatted_line)

# write to output file
with open(output_file, "w") as f:
    f.writelines(output_lines)

print(f"Output written to {output_file}")
