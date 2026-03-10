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
import pickle
from concurrent.futures import ThreadPoolExecutor, as_completed

SAVE_PATH = 'graphs/results_map.pkl'

def save_fig(name):
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


def plot_one(labels, avg_data, k, name):
  plt.figure(figsize=(8, 6))
  plt.plot(labels if labels else avg_data['n'], avg_data['t_ms'], marker='o')
  plt.xlabel('n')
  plt.xticks(rotation=55)
  plt.ylabel('Average Time (milliseconds)')
  plt.title(f'Average Computation Time over {k} runs of {name}(n)')
  plt.grid(True)
  plt.tight_layout()
  
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

    labels = []
    if len(sys.argv) >= 5:
        labels = sys.argv[4].split(" ")

    print(f"{labels=}")
    csv_command = sys.argv[1]
    k = int(sys.argv[2])
    name = sanitize_filename(sys.argv[3]) if len(sys.argv) > 3 else ""
    max_workers = min(k, os.cpu_count() or 1)

    def run_single():
        result = subprocess.run(
            shlex.split(csv_command),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=True,
            text=True
        )
        return pd.read_csv(io.StringIO(result.stdout))

    dfs = []
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        future_to_index = {executor.submit(run_single): idx + 1 for idx in range(k)}
        for future in as_completed(future_to_index):
            idx = future_to_index[future]
            try:
                dfs.append(future.result())
            except Exception as e:
                for pending in future_to_index:
                    pending.cancel()
                print(f"Error running command or reading CSV on iteration {idx}: {e}")
                sys.exit(1)

    all_data = pd.concat(dfs)
    avg_data = all_data.groupby('n', as_index=False)['t'].mean()
    avg_data = avg_data.sort_values('n').reset_index(drop=True)
    avg_data['t_ms'] = avg_data['t'] / 1_000_000

    plot_one(labels, avg_data, k, name)
    update_results_map(name, avg_data)
    
    save_fig(name)
    print(avg_data.tail(10).to_string(index=False))    

def plot_all():
  labels = sys.argv[2].split(" ")
  df_map = load_results_map()
  
  name = ""
  
  plot_many(df_map, name, labels)
  save_fig(name)
  
def load_results_map(filepath=SAVE_PATH) -> dict:
  if os.path.exists(filepath):
      with open(filepath, 'rb') as f:
          results_map = pickle.load(f)
  else:
      results_map = {}
  return results_map

def update_results_map(name, new_df, filepath=SAVE_PATH):
   
    results_map = load_results_map(filepath)

    # 2. Add/Overwrite the new results
    results_map[name] = new_df

    # 3. Save back to disk
    with open(filepath, 'wb') as f:
        pickle.dump(results_map, f)
    
def plot_many(results_map, title, labels):
  
    """
    plt.figure(figsize=(8, 6))
    plt.plot(labels if labels else avg_data['n'], avg_data['t_ms'], marker='o')
    plt.xlabel('n')
    plt.xticks(rotation=55)
    plt.ylabel('Average Time (milliseconds)')
    plt.title(f'Average Computation Time over {k} runs of {name}(n)')
    plt.grid(True)
    plt.tight_layout()
    """
  
    plt.figure(figsize=(8, 6))
    
    # Iterate over the map: name is the key, df is the value
    for method_name, df in results_map.items():
        # Plot each one on the same axes
        # Note: We use df['n'] for the x-axis to ensure 
        # alignment even if methods have different input sizes
        plt.plot(labels, df['t_ms'], marker='o', label=method_name)

    plt.xlabel('Input Size (n)')
    plt.xticks(rotation=55)
    plt.ylabel('Average Time (milliseconds)')
    plt.title(title)
    
    # Very important for multi-plots: show the legend so we know which line is which!
    plt.legend() 
    
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    
    
if __name__ == "__main__":
  
    if len(sys.argv) == 3 and sys.argv[1] == "plot":
      plot_all()
    else:
      main()