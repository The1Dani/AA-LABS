import sys
import numpy as np

"""
Test cases needed for the lab

1. Sorted
2. Reverse_Sorted
3. Randomized
4. sorted but then make n inversions in randomly selected elements and see how changing n affects the run time

Consulted - https://stackoverflow.com/questions/40185647/generating-data-to-test-sorting-algorithm
"""
def random_number_arr(len:int):
  return np.random.randint(-3500, 3500, len).tolist()

def to_formatted_string(arr):
  return f"\"{" ".join(map(str, arr))}\""

def swap(arr, i, j):
  arr[i], arr[j] = arr[j], arr[i]

def randomly_swap(arr, n):
  new_arr = arr[:]
  for _ in range(n):
    i, j = np.random.randint(0, len(arr), 2)
    swap(new_arr, i, j)
  return new_arr
  
if __name__ == "__main__":
  arr15 = random_number_arr(15)
  arr50 = random_number_arr(50)
  arr500 = random_number_arr(500)
  arr5000 = random_number_arr(5_000)
  
  arrays = {
    "random_15": arr15,
    "sorted_15": sorted(arr15),
    "rev_sorted_15": sorted(arr15)[::-1],
    "sorted_10_swap_15": randomly_swap(sorted(arr15), 10),
    "rev_sorted_10_swap_15": randomly_swap(sorted(arr15[::-1]), 10),
    "random_50": arr50,
    "sorted_50": sorted(arr50),
    "rev_sorted_50": sorted(arr50)[::-1],
    "sorted_10_swap_50": randomly_swap(sorted(arr50), 10),
    "rev_sorted_10_swap_50": randomly_swap(sorted(arr50[::-1]), 10),
    "random_500": arr500,
    "sorted_500": sorted(arr500),
    "rev_sorted_500": sorted(arr500)[::-1],
    "sorted_10_swap_500": randomly_swap(sorted(arr500), 10),
    "rev_sorted_10_swap_500": randomly_swap(sorted(arr500[::-1]), 10),
    # "random_5000": arr5000,
    # "sorted_5000": sorted(arr5000),
    # "rev_sorted_5000": sorted(arr5000)[::-1],
    # "sorted_10_swap_5000": randomly_swap(sorted(arr5000), 10),
    # "rev_sorted_10_swap_5000": randomly_swap(sorted(arr5000[::-1]), 10),
    
  }
  
  
  if len(sys.argv) == 1:
    for key, value in arrays.items():
      print(to_formatted_string(value))
  
  elif sys.argv[1] == "labels":
    for key in arrays.keys():
      print(key)