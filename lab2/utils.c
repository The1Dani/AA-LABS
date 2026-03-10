#include "include/utils.h"
#include <stdlib.h>
#include <string.h>

bool sorted(arr_t arr, _compar cmp) {
    for (int i = 1; i < arr.n; ++i) {
        bool res = cmp(at(arr, i - 1, void), at(arr, i, void));
        if (!res)
            return false;
    }
    return true;
}

void assign_bytes(void *to, size_t to_i, void *from, size_t from_i,
                  size_t byte_size) {

    void *to_ptr = to + (byte_size * to_i);
    void *from_ptr = from + (byte_size * from_i);
    memcpy(to_ptr, from_ptr, byte_size);
}

// void swap_bytes(void *a, size_t aind, void *b, size_t bind ,size_t byte_size) {
//     void *temp = malloc(byte_size);
//     // t = a; a = b; b = t;
//     void *ptr_a = (a + aind*byte_size);
//     void *ptr_b = (b + bind*byte_size);
    
//     assign_bytes(temp, 0, ptr_a, 0, byte_size);
//     assign_bytes(ptr_a, 0, ptr_b, 0, byte_size);
//     assign_bytes(ptr_b, 0, temp, 0, byte_size);
    
//     free(temp);
// }
