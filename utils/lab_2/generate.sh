#!/usr/bin/bash
# set -x

#script directory
script_dir=$(dirname "$0")
bin=$script_dir/../../bin/lab2_main
script=$script_dir/main.py

methods=(quick_sort merge_sort heap_sort smooth_sort quick_sort_glibc)

eval "xs=($(python3 "$script_dir/test_cases.py"))"

# echo xs = "\"${xs[*]}\""

labels=(
    $(python3 $script_dir/test_cases.py labels)
)

xss=""

for x in "${xs[@]}"; do
    xss+="\"$x\" "
done

repeat=100
#given a space separated list of methods generate their graphs
for method in "${methods[@]}"; do
    echo "Generating graph for $method"
    cmd="$bin $method $xss"
    # echo "$cmd"
    python3 $script "$cmd" $repeat "$method" "${labels[*]}"
done

python3 "$script" 'plot' "${labels[*]}"