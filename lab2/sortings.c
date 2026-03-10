#include "include/sortings.h"
#include "include/utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
 * COMPARISON FUNCTIONS
 */

int cmp_int_inc(const void *a, const void *b) {
    if (*(int *)b >= *(int *)a)
        return true;
    return false;
}

int cmp_int_dec(const void *a, const void *b) {
    if (*(int *)b <= *(int *)a)
        return true;
    return false;
}

/*
 * HELPERS
 */

// partition function

int __partition(void* arr, int low, int high, size_t byte_size, _compar cmp) {

    // Choose the pivot
    // int pivot = arr[high];
    void *pivot = malloc(byte_size);
    assign_bytes(pivot, 0, arr, high, byte_size);
    
    // Index of smaller element and indicates
    // the right position of pivot found so far
    int i = low - 1;

    // Traverse arr[low..high] and move all smaller
    // elements to the left side. Elements from low to
    // i are smaller after every iteration
    for (int j = low; j <= high - 1; j++) {
        // arr[j] < pivot
        if (cmp((arr + byte_size*j), pivot)) {
            i++;
            swap_bytes(arr + i*byte_size, arr + j*byte_size, byte_size);
        }
    }

    // Move pivot after smaller elements and
    // return its position
    swap_bytes(arr + (i+1)*byte_size,arr + high*byte_size, byte_size);
    
    free(pivot);
    return i + 1;
}

void _merge(void *arr, int l, int m, int r, size_t byte_size, _compar cmp) {
    int left_len = m - l + 1;
    int right_len = r - m;

    void *temp_left = malloc(byte_size * left_len);
    void *temp_right = malloc(byte_size * right_len);

    int i, j, k;

    for (int i = 0; i < left_len; i++)
        assign_bytes(temp_left, i, arr, l + i, byte_size);

    for (int i = 0; i < right_len; i++)
        assign_bytes(temp_right, i, arr, (m + i + 1), byte_size);

    for (i = 0, j = 0, k = l; k <= r; k++) {
        // temp_left[i] >= temp_right[j])
        if (i < left_len &&
            (j >= right_len || cmp((void *)(temp_left + i * byte_size),
                                   ((void *)(temp_right + j * byte_size))))) {
            assign_bytes(arr, k, temp_left, i, byte_size);
            i++;
        } else {
            assign_bytes(arr, k, temp_right, j, byte_size);
            j++;
        }
    }
    free(temp_left);
    free(temp_right);
}

void _merge_sort(void *arr, int l, int r, size_t byte_size, _compar cmp) {
    if (l < r) {
        int m = l + (r - l) / 2;
        int m2 = m + 1;

        _merge_sort(arr, l, m, byte_size, cmp);
        _merge_sort(arr, m2, r, byte_size, cmp);

        _merge(arr, l, m, r, byte_size, cmp);
    }
}

// The QuickSort function implementation
void _quick_sort(void* arr, int low, int high, size_t byte_size, _compar cmp) {
    if (low < high) {

        // pi is the partition return index of pivot
        int pivot = __partition(arr, low, high, byte_size, cmp);

        // recursion calls for smaller elements
        // and greater or equals elements
        _quick_sort(arr, low, pivot - 1, byte_size, cmp);
        _quick_sort(arr, pivot + 1, high, byte_size, cmp);
    }
}

//(int arr[], int n, int i)
void _heapify(arr_t arr, int new_len ,int i, _compar cmp){

    // Initialize largest as root
    int largest = i;

    // left index = 2*i + 1
    int l = 2 * i + 1;

    // right index = 2*i + 2
    int r = 2 * i + 2;

    // If left child is larger than root
    // cmp(arr[l], arr[largest]) reversed arguments
    if (l < new_len && 
        cmp( at(arr, largest, void), at(arr,l,void) ))
        largest = l;

    // If right child is larger than largest so far
    // // cmp(arr[r], arr[largest]) reversed arguments
    if (r < new_len && 
        cmp( at(arr, largest, void), at(arr,r,void) ))
        largest = r;

    // If largest is not root
    if (largest != i) {
        //swap arr[i] and arr[largest]
        swap_bytes(_at(arr, i), _at(arr, largest), arr.byte_size);

        // Recursively heapify the affected sub-tree
        _heapify(arr, new_len, largest, cmp);
    }
}

/*
 * EXPOSED FUNCTIONS
 */

void quick_sort(arr_t arr, _compar cmp) {
    _quick_sort(arr.arr, 0, arr.n - 1, arr.byte_size, cmp);
}

void merge_sort(arr_t arr, _compar cmp) {
    int zero = 0;
    int last = arr.n - 1;
    _merge_sort(arr.arr, zero, last, arr.byte_size, cmp);
}

// Main function to do heap sort
void heap_sort(arr_t arr, _compar cmp){
    
    // Build heap (rearrange vector)
    for (int i = arr.n / 2 - 1; i >= 0; i--)
        _heapify(arr, arr.n, i, cmp);

    // One by one extract an element from heap
    for (int i = arr.n - 1; i > 0; i--) {

        // Move current root to end
        // Swap arr[0] with arr[i]
        swap_bytes(_at(arr,0), _at(arr,i), arr.byte_size);
        
        // Call max heapify on the reduced heap
        _heapify(arr, i, 0, cmp);
    }
}

void gnome_sort(arr_t arr, _compar cmp){
    
    int pos = 1;
    
    while(pos < arr.n) {
        // cmp function indexes swapped to reverse the resulting array 
        // in order to be consistent
        if (pos == 0 || cmp(at(arr, pos, void), at(arr, pos -1, void)) )
            pos++;
        else {
            //pos, pos-1
          swap_bytes(_at(arr,pos), _at(arr,pos-1), arr.byte_size);
          pos--;
        } 
    }

}

void quick_sort_glibc(arr_t arr, _compar cmp) {
    qsort(arr.arr, arr.n, arr.byte_size, cmp);
}