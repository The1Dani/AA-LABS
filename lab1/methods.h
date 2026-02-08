#ifndef METHODS
#define METHODS

#include <gmp.h>
#include <stdint.h>

/*
 * Fibonacci Function Signature
 */
typedef void (*fibo_func)(mpz_t, uint64_t);

/*
 * Result Structure Used in main.c to store results
 */
typedef struct {
    size_t len;
    uint64_t *inputs;
    uint64_t *ms_times;
    mpz_t *returns;
} result_t;

/*
 * Recursive Implementation of naive Fibonacci Sequence
 * Input: n - The Fibonacci index to compute
 * Output: result - The computed Fibonacci number
 * Complexity: O(2^n)
 */
void recursive_fibo(mpz_t result, uint64_t n);

/*
 * Dynamic Programming Implementation of Fibonacci Sequence
 * Input: n - The Fibonacci index to compute
 * Output: result - The computed Fibonacci number
 * Complexity: O(n)
 */
void dp_fibo(mpz_t result, uint64_t n);

/*
 * Matrix Exponentiation Implementation of Fibonacci Sequence
 * Input: n - The Fibonacci index to compute
 * Output: result - The computed Fibonacci number
 * Complexity: O(n)
 */
void matrix_fibo(mpz_t result, uint64_t n);

/*
 * Binet's Formula Implementation of Fibonacci Sequence
 * Input: n - The Fibonacci index to compute
 * Output: result - The computed Fibonacci number
 * Complexity: O(1)
 * Note: Binet's Formula starts to fail for large n due to precision loss.
 */
void binet_fibo(mpz_t result, uint64_t n);

/*
 * Doubling Implementation of Fibonacci Sequence
 * Input: n - The Fibonacci index to compute
 * Output: result - The computed Fibonacci number
 * Complexity: O(log n)
 */
void doubling_fibo(mpz_t result, uint64_t n);

#endif
