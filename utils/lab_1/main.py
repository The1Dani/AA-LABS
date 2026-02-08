import profile
import sys
import pandas as pd
import matplotlib.pyplot as plt
import subprocess
import shlex
import io
import os
import glob
import re

def sanitize_filename(name):
    # Remove any characters that are not alphanumeric, dash, or underscore
    return re.sub(r'[^a-zA-Z0-9_-]', '_', name)

def get_next_graph_number(graphs_dir='./graphs', name=""):
    if name:
        pattern = os.path.join(graphs_dir, f'[0-9][0-9][0-9]_{name}.png')
        regex = re.compile(rf'(\d{{3}})_{re.escape(name)}\.png$')
    else:
        pattern = os.path.join(graphs_dir, '[0-9][0-9][0-9].png')
        regex = re.compile(r'(\d{3})\.png$')
    files = glob.glob(pattern)
    numbers = []
    for f in files:
        base = os.path.basename(f)
        match = regex.match(base)
        if match:
            numbers.append(int(match.group(1)))
    if numbers:
        return max(numbers) + 1
    else:
        return 0

def main():
    if len(sys.argv) < 3:
        print("Usage: python main.py '<csv_command>' k [name]")
        sys.exit(1)

    csv_command = sys.argv[1]
    k = int(sys.argv[2])
    name = sanitize_filename(sys.argv[3]) if len(sys.argv) > 3 else ""

    dfs = []
    for i in range(k):
        try:
            # Run the command and capture its stdout
            # print(f"Running command {i+1}: {csv_command}")
            result = subprocess.run(
                shlex.split(csv_command),
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=True,
                text=True
            )
            df = pd.read_csv(io.StringIO(result.stdout))
            dfs.append(df)
        except Exception as e:
            print(f"Error running command or reading CSV on iteration {i+1}: {e}")
            sys.exit(1)

    # Concatenate all dataframes
    all_data = pd.concat(dfs)
    # Group by 'n' and average 't'
    avg_data = all_data.groupby('n', as_index=False)['t'].mean()
    # Convert nanoseconds to milliseconds
    avg_data['t_ms'] = avg_data['t'] / 1_000_000

    plt.figure(figsize=(8, 6))
    plt.plot(avg_data['n'], avg_data['t_ms'], marker='o')
    plt.xlabel('n')
    plt.ylabel('Average Time (milliseconds)')
    plt.title(f'Average Computation Time over {k} runs of {name}(n)')
    plt.grid(True)
    plt.tight_layout()

    # Ensure the graphs directory exists
    os.makedirs('./graphs', exist_ok=True)
    # Find the next available number for the filename, per name
    next_num = get_next_graph_number('./graphs', name)
    if name:
        out_path = f'./graphs/{next_num:03d}_{name}.png'
    else:
        out_path = f'./graphs/{next_num:03d}.png'
    plt.savefig(out_path)
    print(f"Plot saved to {out_path}")
    print(avg_data.tail(10).to_string(index=False))

if __name__ == "__main__":
    main()
