#!/usr/bin/bash

methods=(dp_fibo matrix_fibo binet_fibo doubling_fibo)
xs='(501, 631, 794, 1000, 1259, 1585, 1995, 2512, 3162, 3981, 5012, 6310, 7943, 10000, 12589, 15849)'
repeat=100
#script directory
script_dir=$(dirname "$0")

bin=$script_dir/../../bin/lab1_main
script=$script_dir/main.py

#given a space separated list of methods generate their graphs
for method in "${methods[@]}"; do
    echo "Generating graph for $method"
    cmd="$bin $method \"$xs\""
    python3 $script "$cmd" $repeat "$method"
done

xs_rec='(5, 7, 10, 12, 15, 17, 20, 22, 25, 27, 30, 32, 35, 37, 40, 42, 45)'
method="recursive_fibo"
repeat=1
echo "Generating graph for $method"
cmd="$bin $method \"$xs_rec\""
python3 $script "$cmd" $repeat "$method"
