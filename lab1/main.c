#include "methods.h"
#include <assert.h>
#include <gmp.h>
#include <libsrt/svector.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BILLION 1000000000LL

/*
 * ./lab1_main method_name seperated_values
 * Format for seperated_values:
 * anything seperated with ', ()' will work
 * the most clean variations are
 * (1 2 3 4)
 * 1, 2, 3, 4
 * (1, 2, 3, 4)
 * -Note that the input format should be one contigious string  
 * 
 * The output format:
 * n,f(x),t
 * n being the input
 * f(x) being the result
 * t the cpu time in nano seconds
 */

const char *methods[] = {
    "recursive_fibo",
    "dp_fibo",
    "matrix_fibo",
    "binet_fibo",
    "doubling_fibo",
};

fibo_func method_funcs[] = {
    recursive_fibo,
    dp_fibo,
    matrix_fibo,
    binet_fibo,
    doubling_fibo,
};

const int methods_len = sizeof(methods) / sizeof(*methods);

void help() {
    printf("./main method_name [start] end\n");
    printf("Usable Methods:\n");
    for (int i = 0; i < methods_len; i++) {
        printf("%s\n", methods[i]);
    }
}

fibo_func get_fibo_func(char *method) {
    for (int i = 0; i < methods_len; i++) {
        if (strcmp(methods[i], method) == 0) {
            return method_funcs[i];
        }
    }

    return NULL;
}

uint64_t get_func_time(fibo_func func, int n, mpz_t ret) {

    struct timespec start, end;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    func(ret, n);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    uint64_t diff_ns =
        BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);

    return diff_ns;
}

result_t asses(fibo_func func, uint64_t *xs, uint64_t xlen) {
    int total_res_num = xlen;
    assert(total_res_num > 0);

    uint64_t *result_times = malloc(sizeof(uint64_t) * total_res_num);
    mpz_t *results = malloc(sizeof(mpz_t) * total_res_num);
    if (!result_times || !results) {
        assert(0 || "No memmory allocated\n");
    }
    for (int i = 0; i < total_res_num; i++) {
        mpz_init(results[i]);
    }

    result_t res = (result_t){
        .len = total_res_num,
        .inputs = xs,
        .ms_times = result_times,
        .returns = results,
    };

    mpz_t ret;
    mpz_init(ret);
    mpz_init_set_ui(ret, 0);

    for (size_t i = 0; i < total_res_num; i++) {
        // printf("%zu %d\n", xs[i], total_res_num);
        result_times[i] = get_func_time(func, xs[i], ret);
        mpz_set(results[i], ret);
    }

    return res;
}

void print_result(result_t res) {
    printf("n,fx,t\n");
    for (int i = 0; i < res.len; i++) {
        gmp_printf("%d,%Zd,%zu\n", res.inputs[i], res.returns[i],
                   res.ms_times[i]);
    }
}

size_t parse_inputs(char *inputs, uint64_t **out) {
    const char *delims = " ,()";
    char *token;

    token = strtok(inputs, delims);
    srt_vector *nums = sv_alloc_t(SV_U64, 2);

    while (token != NULL) {
        sv_push_u64(&nums, atol(token));
        token = strtok(NULL, delims);
    }

    size_t len = sv_len(nums);
    *out = malloc(sizeof(uint64_t) * len);
    for (size_t i = 0; i < len; i++)
        (*out)[i] = sv_at_u64(nums, i);

    return len;
}

int main(int argc, char **argv) {

    fibo_func fibo = 0;
    uint64_t *inputs;
    size_t input_len;

    if (argc < 3 || argc > 3) {
        help();
        exit(1);
    }
    if (argc == 3)
        input_len = parse_inputs(argv[2], &inputs);

    // printf("xs: ");
    // for (int i = 0; i < input_len; i++)
    //     printf("%zu ", inputs[i]);
    // printf("\n");

    fibo = get_fibo_func(argv[1]);

    if (fibo == NULL) {
        printf("The method could not be found!\n");
        help();
        exit(1);
    }

    result_t res = asses(fibo, inputs, input_len);

    print_result(res);

    return 0;
}
