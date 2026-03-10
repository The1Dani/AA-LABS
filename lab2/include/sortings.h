#ifndef SORTINGS_H
#define SORTINGS_H

#include "utils.h"

/*
 * The quick sort algorithm works in the following way,
 * You choose a pivot and the remaining elements will be put in two
 * different chunks one is the smaller than and the other is greater than 
 * and you recursivly apply this until you remain with either one element or no elements in the chunks
 * because one element and no element lists are sorted lists
 * after you do that when you merge togheter the lists you will remain with a fully sorted list
 * "merging together" is just stopping because we dont actually move the elements somewhere they are already in the buffer
 * 
 * 1- Choose pivot
 * 2- Put the elements less than or equals to first chunk (left)
 * 3- Put the elements greater than to second chunk (right)
 * 4- Goto step 1 with the new chunk until you have 0 or 1 elements in the chunk 
 * 
 * Average Case Complexity nlogn
 * Best Case Complexity    nlogn
 * Worst Case Complexity   n^2
 */
void quick_sort(arr_t arr, _compar cmp);


void quick_sort_glibc(arr_t arr, _compar cmp);

/*
 * Merge Sort is an algorithm that sorts the array by splitting the array two smaller pieces.
 * How it does that is it divides the array in to two parts from exactly the middle of the array.
 * After splitting the array in the middle it splits the splitted arrays until gets to a piece that one
 * or zero elements aka a sorted array. Then when it moves back up it merges these hence the name merge
 * sort. To merge these arrays it starts from the start of the one of arrays from the left or the right.
 * it compares the elements and puts them accordingly. 
 * This way it merges the array back but this approach has to allocates memmory 
 * because it cannot do this merging inplace so while the running complexity is really good 
 * but it fells short for the space complexity. 
 * On huge arrays this is really inneficient so this is probably why 
 * it is not used for any libc or standard library implementations. 
 * 
 * 1- Split the array in two from the middle
 * 2- if the array is bigger than one element repeat
 * 3- merge the two halves seperated going up recursively
 * 
 * Merging Algorithm:
 * 1- Select 1 element from each half and compare
 * 2- Put the smaller one on the merging spot and remove from the temp array
 * 3- Repeat Until you remain with no elements in both arrays
 * 4- Repeat for all stages
 * 
 * Average Case Complexity nlogn
 * Best Case Complexity    nlogn
 * Worst Case Complexity   nlogn
 * 
 */
void merge_sort(arr_t arr, _compar cmp);


/*
 * 
 */
void heap_sort(arr_t arr, _compar cmp);

void smooth_sort(arr_t arr, _compar cmp);

/*Compare Functions*/
int cmp_int_inc(const void *a, const void *b);
int cmp_int_dec(const void *a, const void *b);

#endif
