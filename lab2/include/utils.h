#ifndef SORTING_UTILS
#define SORTING_UTILS

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    void *arr;
    size_t n;
    size_t byte_size;
} arr_t;
typedef struct {
    size_t len;
    arr_t *inputs;
    uint64_t *ms_times;
} result_t;

typedef int (*_compar)(const void *, const void *);
typedef void (*_sort_func)(arr_t, _compar);

#define makeArr(p_arr, size)                                                   \
    (arr_t) { .arr = (p_arr), .n = (size), .byte_size = sizeof(*(p_arr)), }

#define allocArr(size, type)                                                   \
    (arr_t) {                                                                  \
        .arr = malloc(size * sizeof(type)), .n = (size),                       \
        .byte_size = sizeof(type),                                             \
    }

#define allocArrArr(size)                                                      \
    (arr_t_arr_t) {                                                            \
        .arr = malloc(size * sizeof(arr_t_arr_t)), .n = (size),                \
        .byte_size = sizeof(arr_t_arr_t),                                      \
    }

#define freeArr(a) free(a.arr);

// Returns the pointer of the array at index i
#define _at(a, i) (((a).arr + ((a).byte_size * (i))))
#define at(a, i, type) ((type *)_at(a,i))

// Assign to index i of the array val of type T
#define assign(a, i, val, T) ((T *)(a).arr)[i] = val;

void assign_bytes(void *to, size_t to_i, void *from, size_t from_i,
                  size_t byte_size);

// void swap_bytes(void *a, size_t aind, void *b, size_t bind ,size_t
// byte_size);

#define swap_bytes(a, b, size)                                                 \
    do {                                                                       \
        register size_t __size = (size);                                       \
        register char *__a = (a), *__b = (b);                                  \
        do {                                                                   \
            char __tmp = *__a;                                                 \
            *__a++ = *__b;                                                     \
            *__b++ = __tmp;                                                    \
        } while (--__size > 0);                                                \
    } while (0)

bool sorted(arr_t arr, _compar cmp);

#endif
