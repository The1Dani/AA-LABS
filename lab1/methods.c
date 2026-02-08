#include "methods.h"
#include <gmp.h>
#include <libsrt/svector.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

void recursive_fibo(mpz_t result, uint64_t n) {

    if (n <= 1) {
        mpz_set_ui(result, n);
        return;
    }

    mpz_t a, b;
    mpz_init(a);
    mpz_init(b);

    recursive_fibo(a, n - 1);
    recursive_fibo(b, n - 2);

    mpz_add(result, a, b);

    mpz_clear(a);
    mpz_clear(b);
}

void dp_fibo(mpz_t result, uint64_t n) {

    if (n <= 1) {
        mpz_set_ui(result, n);
        return;
    }

    mpz_t *vec = malloc(sizeof(mpz_t) * (n + 1));
    for (int i = 0; i < n + 1; i++) {
        mpz_init(vec[i]);
    }

    mpz_set_ui(vec[0], 0);
    mpz_set_ui(vec[1], 1);

    for (int i = 2; i < n + 1; ++i) {
        mpz_add(vec[i], vec[i - 1], vec[i - 2]);
    }

    mpz_set(result, vec[n]);
    for (int i = 0; i < n + 1; i++) {
        mpz_clear(vec[i]);
    }
    free(vec);
}

void multiply(mpz_t f[2][2], mpz_t m[2][2]) {
    mpz_t res[2][2];
    // Initialize result matrix
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            mpz_init(res[i][j]);

    // Perform matrix multiplication
    mpz_t temp;
    mpz_init(temp);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            mpz_set_ui(res[i][j], 0);
            for (int k = 0; k < 2; ++k) {
                mpz_mul(temp, f[i][k], m[k][j]);
                mpz_add(res[i][j], res[i][j], temp);
            }
        }
    }
    mpz_clear(temp);

    // Copy result back to f
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            mpz_set(f[i][j], res[i][j]);

    // Clear result matrix
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            mpz_clear(res[i][j]);
}

//O(n) Algorithm for matrix power
void power(mpz_t f[2][2], uint64_t n) {
    if (n <= 1)
        return;

    // This is the base transformation matrix m
    mpz_t m[2][2];
    mpz_init_set_ui(m[0][0], 1);
    mpz_init_set_ui(m[0][1], 1);
    mpz_init_set_ui(m[1][0], 1);
    mpz_init_set_ui(m[1][1], 0);

    for (uint64_t i = 1; i < n; i++) {
        multiply(f, m);
    }

    // Clean up local matrix m
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            mpz_clear(m[i][j]);
}

void matrix_fibo(mpz_t result, uint64_t n) {
    if (n <= 1) {
        mpz_set_ui(result, n);
        return;
    }

    mpz_t f[2][2];
    // Proper initialization
    mpz_init_set_ui(f[0][0], 1);
    mpz_init_set_ui(f[0][1], 1);
    mpz_init_set_ui(f[1][0], 1);
    mpz_init_set_ui(f[1][1], 0);

    power(f, n - 1);

    mpz_set(result, f[0][0]);

    // Clean up
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            mpz_clear(f[i][j]);
}


void binet_fibo_init(mpf_t phi_mpf, mpf_t phi1_mpf, mpf_t sqrt5_mpf,
                     mpf_t result_mpf, mpf_t temp_mpf, mpf_t mpf_2) {

    double sqrt5 = sqrt(5);
    mpf_init_set_d(phi_mpf, 1 + sqrt5);
    mpf_init_set_d(phi1_mpf, 1 - sqrt5);
    mpf_init_set_d(sqrt5_mpf, sqrt5);
    mpf_init(result_mpf);
    mpf_init(temp_mpf);
    mpf_init_set_ui(mpf_2, 2);
}

// pow(phi, n) - pow(phi1, n) / ( pow(2, n) * sqrt(5) )
void binet_fibo(mpz_t result, uint64_t n) {
    if (n <= 1) {
        mpz_set_ui(result, n);
        return;
    }
    mpf_t phi_mpf, phi1_mpf, sqrt5_mpf, result_mpf, temp_mpf, mpf_2;
    binet_fibo_init(phi_mpf, phi1_mpf, sqrt5_mpf, result_mpf, temp_mpf, mpf_2);

    mpf_set_default_prec(256);

    mpf_pow_ui(result_mpf, phi_mpf, n);
    mpf_pow_ui(temp_mpf, phi1_mpf, n);
    mpf_sub(result_mpf, result_mpf, temp_mpf);

    mpf_pow_ui(temp_mpf, mpf_2, n);
    mpf_mul(temp_mpf, temp_mpf, sqrt5_mpf);

    mpf_div(result_mpf, result_mpf, temp_mpf);
    mpz_set_f(result, result_mpf);

    mpf_clear(result_mpf);
    mpf_clear(phi_mpf);
    mpf_clear(phi1_mpf);
    mpf_clear(sqrt5_mpf);
}

/*
 * F(2k)=F(k)[2F(k+1)−F(k)].
 * F(2k+1)=F(k+1)^2+F(k)^2.
 */

typedef struct {
    mpz_t t[2];
} touple;

touple _doubling_fibo(uint64_t n) {

    touple t;
    mpz_init_set_ui(t.t[0], n);
    mpz_init_set_ui(t.t[1], n);
    if (n <= 1) {
        return t;
    }

    touple p = _doubling_fibo(n >> 1);
    mpz_t temp, c, d;
    mpz_init(temp);
    // c
    mpz_mul_ui(temp, p.t[1], 2);
    mpz_sub(temp, temp, p.t[0]);
    mpz_mul(temp, p.t[0], temp);
    mpz_init_set(c, temp);
    // d
    mpz_pow_ui(temp, p.t[0], 2);
    mpz_init_set(d, temp);
    mpz_pow_ui(temp, p.t[1], 2);
    mpz_add(d, d, temp);

    if (n % 2) {
        mpz_set(t.t[0], d);
        mpz_add(t.t[1], c, d);
        return t;
    } else {
        mpz_set(t.t[0], c);
        mpz_set(t.t[1], d);
        return t;
    }
}

void doubling_fibo(mpz_t result, uint64_t n) {

    if (n <= 1) {
        mpz_set_ui(result, n);
        return;
    }

    touple t = _doubling_fibo(n);
    mpz_set(result, t.t[0]);
}
