#include "include/sortings.h"
#include "include/utils.h"
#include <assert.h>
#include <libsrt/svector.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BILLION 1000000000LL
#define KB      1024
#define COMPAR_FN cmp_int_dec

const char *methods[] = {
    "quick_sort",
    "merge_sort",
    "heap_sort",
    "smooth_sort",
    "quick_sort_glibc",
};

_sort_func sorting_funcs[] = {
    quick_sort,
    merge_sort,
    heap_sort,
    smooth_sort,
    quick_sort_glibc,
};

const int methods_len = sizeof(methods) / sizeof(*methods);

void help() {
    printf("./main method_name [start] end\n");
    printf("Usable Methods:\n");
    for (int i = 0; i < methods_len; i++) {
        printf("%s\n", methods[i]);
    }
}

_sort_func get_sort_func(char *method) {
    for (int i = 0; i < methods_len; i++) {
        if (strcmp(methods[i], method) == 0) {
            return sorting_funcs[i];
        }
    }

    return NULL;
}

uint64_t get_func_time(_sort_func func, arr_t arr, _compar cmp) {

    struct timespec start, end;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    func(arr, cmp);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    uint64_t diff_ns =
        BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);

    return diff_ns;
}

result_t asses(_sort_func func, arr_t *inputs, uint64_t xlen) {
    int total_res_num = xlen;
    assert(total_res_num > 0);

    uint64_t *result_times = malloc(sizeof(uint64_t) * total_res_num);
    result_t res = (result_t){
        .len = total_res_num,
        .inputs = inputs,
        .ms_times = result_times,
    };

    for (size_t i = 0; i < total_res_num; i++) {
        result_times[i] =
            get_func_time(func, inputs[i], COMPAR_FN );
    }

    return res;
}

char *arr_to_str(arr_t arr) {
    char *str = calloc(KB*2, sizeof (char));
    char *buf = calloc(KB, sizeof (char));
    
    strcat(str, "(");
    for (int i = 0; i < arr.n; i++) {
        sprintf(buf, "%d, ", *at(arr, i, int));
        strcat(str, buf);
    }
    str[strlen(str) - 2] = '\0';
    strcat(str, ")");
    free(buf);
    return str;
}

void print_result(result_t res, arr_t *inputs, size_t inputs_len) {
    printf("n,t\n");
    for (int i = 0; i < res.len; i++) {
        printf("%d,%zu\n", i + 1,
               res.ms_times[i]/* ,
                   arr_to_str(inputs[i])*/);
    }
}

void parse_arg(char *arg, arr_t *out) {
    const char *delims = " ,()";
    char *token;

    token = strtok(arg, delims);
    srt_vector *nums = sv_alloc_t(SV_I32, 2);

    while (token != NULL) {
        sv_push_i32(&nums, atoi(token));
        token = strtok(NULL, delims);
    }

    size_t len = sv_len(nums);
    arr_t new_out = allocArr(len, int);
    int* buff = (int*)sv_get_buffer(nums);
    
    for (int i = 0; i < new_out.n; ++i) {
        assign(new_out, i, buff[i], int);
    }
    *out = new_out;
}

void parse_inputs(size_t input_len, char **inputs, arr_t **out) {
    // inputs[2] is the first argument
    *out = malloc(sizeof(arr_t) * input_len);
    arr_t* arr_t_arr = *out; 
    
    int i = 2;
    while (inputs[i]) {
        parse_arg(inputs[i], &(arr_t_arr[i-2]) );
        assert(arr_t_arr[i-2].arr);
        ++i;
    }
}

void print_arr_t(arr_t arr) {
    for (int i = 0; i < arr.n; i++) 
        printf("%d\n", *at(arr, i, int));
}

void print_arr_t_arr_t(arr_t* arr, size_t len) {
    for (int i = 0; i < len; i++) {
        bool is_sorted =
        sorted(arr[i], COMPAR_FN);
        // assert(is_sorted);
        printf("\nArg[%d], Sorted = %s\n", i, is_sorted ? "true" : "false");
        print_arr_t(arr[i]);
    }
}

int main(int argc, char **argv) {

    _sort_func sortfunc = 0;
    arr_t* inputs; // arr_t array
    size_t input_len = argc - 2;

    if (argc < 3) {
        help();
        exit(1);
    }
    if (argc >= 3)
        parse_inputs(input_len, argv, &inputs);
    
    sortfunc = get_sort_func(argv[1]);

    if (sortfunc == NULL) {
        printf("The method could not be found!\n");
        help();
        exit(1);
    }

    result_t res = asses(sortfunc, inputs, input_len);
    // print_arr_t_arr_t(inputs, input_len);
    // for (int i = 0; i < input_len; i++){
    //     // printf("%d\n",i);
    //     // print_arr_t_arr_t(inputs, input_len);
    //     assert(sorted(inputs[i], COMPAR_FN));
    // }
        
    print_result(res, inputs, input_len);

    return 0;
}
