#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "fusion.h"
#include "k.h"

#define FNS_L1 \
    _(double, dasum_, (const int64_t* n, const double* dx, const int64_t* incx)) \
    _(void, daxpy_, (const int64_t* n, const double* da, const double* dx, const int64_t* incx, double* dy, const int64_t* incy)) \
    _(void, dcopy_, (const int64_t* n, const double* dx, const int64_t* incx, double* dy, const int64_t* incy)) \
    _(double, ddot_, (const int64_t* n, const double* dx, const int64_t* incx, const double* dy, const int64_t* incy)) \
    _(double, dnrm2_, (const int64_t* n, const double* dx, const int64_t* incx)) \
    _(void, drot_, (const int64_t* n, double* dx, const int64_t* incx, double* dy, const int64_t* incy, const double* c, const double* s)) \
    _(void, drotg_, (double* da, double* db, double* c, double* s)) \
    _(void, drotm_, (const int64_t* n, double* dx, const int64_t* incx, double* dy, const int64_t* incy, const double* dparam)) \
    _(void, drotmg_, (double* dd1, double* dd2, double* dx1, const double* dy1, double* dparam)) \
    _(void, dscal_, (const int64_t* n, const double* da, double* dx, const int64_t* incx)) \
    _(void, dswap_, (const int64_t* n, double* dx, const int64_t* incx, double* dy, const int64_t* incy)) \
    _(int64_t, idamax_, (const int64_t* n, const double* dx, const int64_t* incx))

#define FNS_L2 \
    _(void, dgemv_, (char*, int64_t*, int64_t*, double*, double*, int64_t*, double*, int64_t*, double*, double*, int64_t*)) \
    _(void, dgbmv_, (char*, int64_t*, int64_t*, int64_t*, int64_t*, double*, double*, int64_t*, double*, int64_t*, double*, double*, int64_t*)) \
    _(void, dsymv_, (char*, int64_t*, double*, double*, int64_t*, double*, int64_t*, double*, double*, int64_t*)) \
    _(void, dsbmv_, (char*, int64_t*, int64_t*, double*, double*, int64_t*, double*, int64_t*, double*, double*, int64_t*)) \
    _(void, dspmv_, (char*, int64_t*, double*, double*, double*, int64_t*, double*, double*, int64_t*)) \
    _(void, dtrmv_, (char*, char*, char*, int64_t*, double*, int64_t*, double*, int64_t*)) \
    _(void, dtbmv_, (char*, char*, char*, int64_t*, int64_t*, double*, int64_t*, double*, int64_t*)) \
    _(void, dtpmv_, (char*, char*, char*, int64_t*, double*, double*, int64_t*)) \
    _(void, dtrsv_, (char*, char*, char*, int64_t*, double*, int64_t*, double*, int64_t*)) \
    _(void, dtbsv_, (char*, char*, char*, int64_t*, int64_t*, double*, int64_t*, double*, int64_t*)) \
    _(void, dtpsv_, (char*, char*, char*, int64_t*, double*, double*, int64_t*)) \
    _(void, dger_, (int64_t*, int64_t*, double*, double*, int64_t*, double*, int64_t*, double*, int64_t*))

#define FNS_L3 \
    _(void, dgemm_, (char*, char*, int64_t*, int64_t*, int64_t*, double*, double*, int64_t*, double*, int64_t*, double*, double*, int64_t*)) \
    _(void, dsymm_, (char*, char*, int64_t*, int64_t*, double*, double*, int64_t*, double*, int64_t*, double*, double*, int64_t*)) \
    _(void, dsyrk_, (char*, char*, int64_t*, int64_t*, double*, double*, int64_t*, double*, double*, int64_t*)) \
    _(void, dsyr2k_, (char*, char*, int64_t*, int64_t*, double*, double*, int64_t*, double*, int64_t*, double*, double*, int64_t*)) \
    _(void, dtrmm_, (char*, char*, char*, char*, int64_t*, int64_t*, double*, double*, int64_t*, double*, int64_t*)) \
    _(void, dtrsm_, (char*, char*, char*, char*, int64_t*, int64_t*, double*, double*, int64_t*, double*, int64_t*))

#define _(r, n, a) Z r (*n)a;
FNS_L1
FNS_L2
FNS_L3
#undef _

/* HELPERS */

// Convert KDB/Q object to a flat double array
ZF* k_to_double_array(K x) {
    if (x->t == KF) {
        int64_t n = x->n;
        double* arr = malloc(n * sizeof(double));
        if (!arr) return NULL;
        memcpy(arr, kF(x), n * sizeof(double));
        return arr;
    }
    if (x->t == 0 && kK(x)[0]->t == KF) {
        int64_t rows = x->n;
        int64_t cols = kK(x)[0]->n;
        double* arr = malloc(rows * cols * sizeof(double));
        if (!arr) return NULL;
        for (int64_t i = 0; i < rows; ++i) {
            memcpy(arr + i * cols, kF(kK(x)[i]), cols * sizeof(double));
        }
        return arr;
    }
    return NULL;
}

// Convert KDB/Q object to a column-major double array
ZF* k_to_double_array_column(K k_obj) {
    int64_t rows = k_obj->n;
    int64_t cols = kK(k_obj)[0]->n;
    double* arr = malloc(rows * cols * sizeof(double));
    if (!arr) return NULL;

    for (int64_t i = 0; i < rows; i++) {
        for (int64_t j = 0; j < cols; j++) {
            arr[j * rows + i] = kF(kK(k_obj)[i])[j]; // Column-major order
        }
    }
    return arr;
}

// Convert a flat double array to a KDB/Q vector
ZK double_array_to_k_vector(double* arr, J n) {
    K result = ktn(KF, n);
    if (!result) return NULL;
    memcpy(kF(result), arr, n * sizeof(double));
    return result;
}

// Convert a flat double array to a KDB/Q matrix (row-major)
ZK double_array_to_k_matrix_row(double* arr, J m, J n) {
    K result = ktn(0, m);
    if (!result) return NULL;

    for (int64_t i = 0; i < m; i++) {
        K row = ktn(KF, n);
        if (!row) {
            r0(result);
            return NULL;
        }
        memcpy(kF(row), arr + i * n, n * sizeof(double));
        kK(result)[i] = row;
    }
    return result;
}

// Convert a flat double array to a KDB/Q matrix (column-major to row-major)
ZK double_array_to_k_matrix(double* arr, int64_t rows, int64_t cols) {
    K result = ktn(0, rows);
    for (int64_t i = 0; i < rows; i++) {
        K row = ktn(KF, cols);
        for (int64_t j = 0; j < cols; j++) {
            kF(row)[j] = arr[j * rows + i]; // Convert column-major to row-major
        }
        kK(result)[i] = row;
    }
    return result;
}

// Transpose a matrix
ZF* transpose_matrix(double* arr, int64_t m, int64_t n) {
    double* transposed = malloc(m * n * sizeof(double));
    if (!transposed) return NULL;

    for (int64_t i = 0; i < m; i++) {
        for (int64_t j = 0; j < n; j++) {
            transposed[j * m + i] = arr[i * n + j];
        }
    }
    return transposed;
}

// Find the index of a key in a KDB/Q dictionary
Z int64_t find_key(K keys, S key) {
    for (int64_t i = 0; i < keys->n; i++) {
        if (!strcmp(kS(keys)[i], key)) return i;
    }
    return -1;  // Key not found
}

// Get a value from a KDB/Q dictionary
ZK get_value_from_dict(K params, char* key) {
    K keys = kK(params)[0];
    K values = kK(params)[1];

    int64_t idx = find_key(keys, key);
    if (idx < 0 || idx >= keys->n) {
        return 0;
    }
    return kK(values)[idx];
}

// Get a double value from a KDB/Q dictionary
ZF get_double_value(K params, char* key, double default_value) {
    K value = get_value_from_dict(params, key);
    if (!value || value->t != -KF) {
        return default_value;
    }
    double result = value->f;
    return result;
}

// Get a character value from a KDB/Q dictionary
ZC get_char_value(K params, char* key, char default_value) {
    K value = get_value_from_dict(params, key);
    if (!value || value->t != -KC) {
        return default_value;
    }
    char result = value->g;
    return result;
}

// Get an integer value from a KDB/Q dictionary
Z int64_t get_int_value(K params, char* key, int64_t default_value) {
    K value = get_value_from_dict(params, key);
    if (!value || value->t != -KI) {
        return default_value;
    }
    int64_t result = value->i;
    return result;
}

/* LEVEL 1 OPENBLAS FUNCTIONS */

// Sum of absolute values -> |x1| + |x2| + ... + |xn|
Z K1(z_dasum) {
    P(xt != KF, krr("type"));
    int64_t n = xn;
    int64_t inc = 1;
    double result = dasum_(&n, kF(x), &inc);
    return kf(result);
}

// Vector addition -> y = αx + y
ZK z_daxpy(K x_vec, K y_vec, K alpha) {
    P(x_vec->t != KF || y_vec->t != KF, krr("type"));
    P(x_vec->n != y_vec->n, krr("length"));
    int64_t n = x_vec->n;
    int64_t incx = 1, incy = 1;
    double alpha_val = alpha->f;
    double* result = malloc(n * sizeof(double));
    memcpy(result, kF(y_vec), n * sizeof(double));
    daxpy_(&n, &alpha_val, kF(x_vec), &incx, result, &incy);
    K r = ktn(KF, n);
    memcpy(kF(r), result, n * sizeof(double));
    free(result);
    return r;
}

// Copy vector -> y = x
ZK z_dcopy(K x_vec, K y_vec) {
    P(x_vec->t != KF || y_vec->t != KF, krr("type"));
    P(x_vec->n != y_vec->n, krr("length"));
    int64_t n = x_vec->n;
    int64_t incx = 1, incy = 1;
    K r = ktn(KF, n);
    memcpy(kF(r), kF(x_vec), n * sizeof(double));
    return r;
}

// Dot product -> x.y
ZK z_ddot(K x_vec, K y_vec) {
    P(x_vec->t != KF || y_vec->t != KF, krr("type"));
    P(x_vec->n != y_vec->n, krr("length"));
    P(x_vec->n == 0, kf(0.0));
    int64_t n = x_vec->n;
    int64_t incx = 1, incy = 1;
    double result = ddot_(&n, kF(x_vec), &incx, kF(y_vec), &incy);
    return kf(result);
}

// Euclidean norm -> sqrt(x.x)
Z K1(z_dnrm2) {
    P(xt != KF, krr("type"));
    int64_t n = xn;
    int64_t inc = 1;
    double result = dnrm2_(&n, kF(x), &inc);
    return kf(result);
}

// Plane rotation of points
ZK z_drot(K x_vec, K y_vec, K cos, K sin) {
    P(x_vec->t != KF || y_vec->t != KF || cos->t != -KF || sin->t != -KF, krr("type"));
    P(x_vec->n != y_vec->n, krr("length"));
    int64_t n = x_vec->n;
    int64_t incx = 1, incy = 1;
    double cos_val = cos->f;
    double sin_val = sin->f;
    drot_(&n, kF(x_vec), &incx, kF(y_vec), &incy, &cos_val, &sin_val);
    R (K)0; // modified in place
}

// Generate plane rotation
ZK z_drotg(K a, K b) {
    P(a->t != -KF || b->t != -KF, krr("type"));
    double da = a->f;
    double db = b->f;
    double c, s;
    drotg_(&da, &db, &c, &s);
    return knk(4, kf(da), kf(db), kf(c), kf(s));
}

// Modified plane rotation
ZK z_drotm(K x_vec, K y_vec, K param_k) {
    P(x_vec->t != KF || y_vec->t != KF, krr("vector_type"));
    P(param_k->t != KF || param_k->n != 5, krr("param_type"));
    int64_t n = x_vec->n;
    double* x_arr = k_to_double_array(x_vec);
    double* y_arr = k_to_double_array(y_vec);
    double* param = k_to_double_array(param_k);
    if (!x_arr || !y_arr || !param) {
        free(x_arr); free(y_arr); free(param);
        return krr("memory");
    }
    int64_t incx = 1, incy = 1;
    drotm_(&n, x_arr, &incx, y_arr, &incy, param);
    K result_x = double_array_to_k_vector(x_arr, n);
    K result_y = double_array_to_k_vector(y_arr, n);
    free(x_arr); free(y_arr); free(param);
    K result = ktn(0, 2);
    kK(result)[0] = result_x;
    kK(result)[1] = result_y;
    return result;
}

// Generate modified plane rotation
ZK z_drotmg(K d1, K d2, K x1, K y1) {
    P(d1->t != -KF || d2->t != -KF || x1->t != -KF || y1->t != -KF, krr("type"));
    double dd1 = d1->f;
    double dd2 = d2->f;
    double dx1 = x1->f;
    double dy1 = y1->f;
    double dparam[5] = {0};
    drotmg_(&dd1, &dd2, &dx1, &dy1, dparam);
    K result = ktn(KF, 5);
    memcpy(kF(result), dparam, 5 * sizeof(double));
    return result;
}

// Scale a vector -> x = αx
ZK z_dscal(K alpha, K x_vec) {
    P(alpha->t != -KF || x_vec->t != KF, krr("type"));
    int64_t n = x_vec->n;
    int64_t incx = 1;
    double alpha_val = alpha->f;
    K result = ktn(KF, n);
    memcpy(kF(result), kF(x_vec), n * sizeof(double));
    dscal_(&n, &alpha_val, kF(result), &incx);
    return result;
}

// Swap vectors -> x <-> y
ZK z_dswap(K x_vec, K y_vec) {
    P(x_vec->t != KF || y_vec->t != KF, krr("type"));
    P(x_vec->n != y_vec->n, krr("length"));
    int64_t n = x_vec->n;
    int64_t incx = 1, incy = 1;
    dswap_(&n, kF(x_vec), &incx, kF(y_vec), &incy);
    R (K)0;
}

// Index of maximum absolute value
Z K1(z_idamax) {
    P(xt != KF, krr("type"));
    int64_t n = xn;
    int64_t inc = 1;
    int64_t result = idamax_(&n, kF(x), &inc);
    return kj(result - 1);  // Convert from 1-based to 0-based indexing
}

/* LEVEL 2 OPENBLAS FUNCTION */

// General matrix-vector multiplication -> y = alpha*A*x + beta*y
ZK z_dgemv(K matr, K x_vec, K y_vec, K params) {
    P(matr->t != 0 || ((matr->t == 0) && (kK(matr)[0]->t != KF)), krr("matrix_type"));
    P(x_vec->t != KF || y_vec->t != KF, krr("array_type"));
    P(params->t != XD, krr("params_type"));

    double alpha = get_double_value(params, "alpha", 1.0);
    double beta = get_double_value(params, "beta", 1.0);
    char trans = get_char_value(params, "trans", 'N');

    int64_t m = matr->n;
    int64_t n = kK(matr)[0]->n;
    int64_t incx = 1, incy = 1;

    double* a_arr = malloc(m * n * sizeof(double));
    if (!a_arr) return krr("memory");

    for (int64_t i = 0; i < m; i++) {
        for (int64_t j = 0; j < n; j++) {
            a_arr[j * m + i] = kF(kK(matr)[i])[j]; // Column-major order
        }
    }

    double* x_arr = k_to_double_array(x_vec);
    double* y_arr = k_to_double_array(y_vec);
    if (!x_arr || !y_arr) {
        free(a_arr); free(x_arr); free(y_arr);
        return krr("memory");
    }

    dgemv_(&trans, &m, &n, &alpha, a_arr, &m,
           x_arr, &incx, &beta, y_arr, &incy);

    K result = double_array_to_k_vector(y_arr, m);
    free(a_arr); free(x_arr); free(y_arr);
    return result;
}

// Banded matrix-vector multiplication
ZK z_dgbmv(K matr, K x_vec, K y_vec, K params) {
    P(matr->t != 0 || ((matr->t == 0) && (kK(matr)[0]->t != KF)), krr("matrix_type"));
    P(x_vec->t != KF || y_vec->t != KF, krr("array_type"));
    P(params->t != XD, krr("params_type"));

    double alpha = get_double_value(params, "alpha", 1.0);
    double beta = get_double_value(params, "beta", 1.0);
    char trans = get_char_value(params, "trans", 'N');
    int64_t kl = get_int_value(params, "kl", 1);
    int64_t ku = get_int_value(params, "ku", 1);

    int64_t m = matr->n;
    int64_t n = kK(matr)[0]->n;
    int64_t incx = 1, incy = 1;
    int64_t ldab = 2 * kl + ku + 1;

    double* ab_arr = malloc(ldab * n * sizeof(double));
    if (!ab_arr) return krr("memory");
    memset(ab_arr, 0, ldab * n * sizeof(double));

    for (int64_t j = 0; j < n; j++) {
        for (int64_t i = 0; i < m; i++) {
            double value = kF(kK(matr)[i])[j];
            if (i >= j - ku && i <= j + kl) {
                int64_t band_row = kl + (i - j);
                ab_arr[band_row + j * ldab] = value;
            }
        }
    }

    double* x_arr = k_to_double_array(x_vec);
    double* y_arr = k_to_double_array(y_vec);
    if (!x_arr || !y_arr) {
        free(ab_arr); free(x_arr); free(y_arr);
        return krr("memory");
    }

    dgbmv_(&trans, &m, &n, &kl, &ku, &alpha, ab_arr, &ldab,
           x_arr, &incx, &beta, y_arr, &incy);

    K result = double_array_to_k_vector(y_arr, m);
    free(ab_arr); free(x_arr); free(y_arr);
    return result;
}

// Symmetric matrix-vector multiplication
ZK z_dsymv(K matr, K x_vec, K y_vec, K params) {
    P(matr->t != 0 || ((matr->t == 0) && (kK(matr)[0]->t != KF)), krr("matrix_type"));
    P(x_vec->t != KF || y_vec->t != KF, krr("array_type"));
    P(params->t != XD, krr("params_type"));

    double alpha = get_double_value(params, "alpha", 1.0);
    double beta = get_double_value(params, "beta", 1.0);
    char uplo = get_char_value(params, "uplo", 'U');

    int64_t n = matr->n;
    int64_t incx = 1, incy = 1;

    double* a_arr = k_to_double_array(matr);
    double* x_arr = k_to_double_array(x_vec);
    double* y_arr = k_to_double_array(y_vec);
    if (!a_arr || !x_arr || !y_arr) {
        free(a_arr); free(x_arr); free(y_arr);
        return krr("memory");
    }

    dsymv_(&uplo, &n, &alpha, a_arr, &n, x_arr, &incx, &beta, y_arr, &incy);

    K result = double_array_to_k_vector(y_arr, n);
    free(a_arr); free(x_arr); free(y_arr);
    return result;
}

// Symmetric banded matrix-vector multiplication
ZK z_dsbmv(K matr, K x_vec, K y_vec, K params) {
    P(matr->t != 0 || ((matr->t == 0) && (kK(matr)[0]->t != KF)), krr("matrix_type"));
    P(x_vec->t != KF || y_vec->t != KF, krr("array_type"));
    P(params->t != XD, krr("params_type"));

    double alpha = get_double_value(params, "alpha", 1.0);
    double beta = get_double_value(params, "beta", 1.0);
    char uplo = get_char_value(params, "uplo", 'U');
    int64_t ku = get_int_value(params, "ku", 1);

    int64_t n = x_vec->n;
    int64_t ldab = ku + 1;
    int64_t incx = 1, incy = 1;

    double* ab_arr = malloc(ldab * n * sizeof(double));
    double* x_arr = k_to_double_array(x_vec);
    double* y_arr = k_to_double_array(y_vec);
    if (!ab_arr || !x_arr || !y_arr) {
        free(ab_arr); free(x_arr); free(y_arr);
        return krr("memory");
    }

    memset(ab_arr, 0, ldab * n * sizeof(double));
    for (int64_t j = 0; j < n; j++) {
        for (int64_t i = 0; i < n; i++) {
            if (i >= j - ku && i <= j) { // Check bounds for band matrix
                int64_t band_row = ku + i - j;
                ab_arr[band_row + j * ldab] = kF(kK(matr)[i])[j];
            }
        }
    }

    dsbmv_(&uplo, &n, &ku, &alpha, ab_arr, &ldab, x_arr, &incx, &beta, y_arr, &incy);

    K result = double_array_to_k_vector(y_arr, n);
    free(ab_arr); free(x_arr); free(y_arr);
    return result;
}

// Symmetric packed matrix-vector multiplication
ZK z_dspmv(K matr, K x_vec, K y_vec, K params) {
    P(matr->t != KF, krr("matrix_type"));
    P(x_vec->t != KF || y_vec->t != KF, krr("array_type"));
    P(params->t != XD, krr("params_type"));

    double alpha = get_double_value(params, "alpha", 1.0);
    double beta = get_double_value(params, "beta", 1.0);
    char uplo = get_char_value(params, "uplo", 'U');

    int64_t n = x_vec->n;
    int64_t incx = 1, incy = 1;

    double* ap_arr = k_to_double_array(matr);
    double* x_arr = k_to_double_array(x_vec);
    double* y_arr = k_to_double_array(y_vec);
    if (!ap_arr || !x_arr || !y_arr) {
        free(ap_arr); free(x_arr); free(y_arr);
        return krr("memory");
    }

    dspmv_(&uplo, &n, &alpha, ap_arr, x_arr, &incx, &beta, y_arr, &incy);

    K result = double_array_to_k_vector(y_arr, n);
    free(ap_arr); free(x_arr); free(y_arr);
    return result;
}

// Triangular matrix-vector multiplication
ZK z_dtrmv(K matr, K x_vec, K params) {
    P(matr->t != 0 || ((matr->t == 0) && (kK(matr)[0]->t != KF)), krr("matrix_type"));
    P(x_vec->t != KF, krr("array_type"));

    K keys = NULL, values = NULL;
    if (params && params->t == XD) {
        keys = kK(params)[0];
        values = kK(params)[1];
    }

    char uplo = get_char_value(params, "uplo", 'U');
    char trans = get_char_value(params, "trans", 'N');
    char diag = get_char_value(params, "diag", 'N');

    int64_t n = matr->n;
    int64_t incx = 1;

    double* a_arr = k_to_double_array(matr);
    double* x_arr = k_to_double_array(x_vec);
    if (!a_arr || !x_arr) {
        free(a_arr); free(x_arr);
        return krr("memory");
    }

    double* a_arr_transposed = transpose_matrix(a_arr, n, n);
    if (!a_arr_transposed) {
        printf("Error: Failed to transpose matrix\n");
        free(a_arr); free(x_arr);
        return krr("memory");
    }

    dtrmv_(&uplo, &trans, &diag, &n, a_arr_transposed, &n, x_arr, &incx);
    free(a_arr_transposed);

    K result = double_array_to_k_vector(x_arr, n);
    free(a_arr); free(x_arr);
    return result;
}

// Rank-1 update of a general matrix
ZK z_dger(K matr, K x_vec, K y_vec, K alpha_k) {
    P(matr->t != 0 || ((matr->t == 0) && (kK(matr)[0]->t != KF)), krr("matrix_type"));
    P(x_vec->t != KF || y_vec->t != KF, krr("array_type"));
    P(alpha_k->t != -KF, krr("alpha_type"));

    double alpha = alpha_k->f;
    int64_t m = matr->n;
    int64_t n = kK(matr)[0]->n;
    int64_t incx = 1, incy = 1;

    double* a_arr = k_to_double_array(matr);
    double* x_arr = k_to_double_array(x_vec);
    double* y_arr = k_to_double_array(y_vec);

    if (!a_arr || !x_arr || !y_arr) {
        free(a_arr); free(x_arr); free(y_arr);
        return krr("memory");
    }

    double* a_arr_transposed = transpose_matrix(a_arr, m, n);
    if (!a_arr_transposed) {
        free(a_arr); free(x_arr); free(y_arr);
        return krr("memory");
    }

    dger_(&m, &n, &alpha, x_arr, &incx, y_arr, &incy, a_arr_transposed, &m);

    for (int64_t i = 0; i < m; i++) {
        for (int64_t j = 0; j < n; j++) {
            a_arr[i * n + j] = a_arr_transposed[j * m + i];
        }
    }

    free(a_arr_transposed);

    K result = double_array_to_k_matrix_row(a_arr, m, n);
    free(a_arr); free(x_arr); free(y_arr);

    return result;
}

/* LEVEL 3 OPENBLAS FUNCTIONS */

// General matrix-matrix multiplication -> C = alpha*A*B + beta*C
ZK z_dgemm(K A, K B, K C, K params) {
    P(A->t != 0 || ((A->t == 0) && (kK(A)[0]->t != KF)), krr("type"));
    P(B->t != 0 || ((B->t == 0) && (kK(B)[0]->t != KF)), krr("type"));
    P(C->t != 0 || ((C->t == 0) && (kK(C)[0]->t != KF)), krr("type"));
    P(params->t != XD, krr("params_type"));

    double alpha = get_double_value(params, "alpha", 1.0);
    double beta = get_double_value(params, "beta", 1.0);
    char transA = get_char_value(params, "transA", 'N');
    char transB = get_char_value(params, "transB", 'N');

    int64_t m_A = A->n;
    int64_t n_A = kK(A)[0]->n;
    int64_t m_B = B->n;
    int64_t n_B = kK(B)[0]->n;
    int64_t m_C = C->n;
    int64_t n_C = kK(C)[0]->n;

    if ((transA == 'N' && transB == 'N') && (n_A != m_B || m_A != m_C || n_B != n_C)) return krr("dimension_mismatch");
    if ((transA == 'T' && transB == 'N') && (m_A != m_B || n_A != m_C || n_B != n_C)) return krr("dimension_mismatch");
    if ((transA == 'N' && transB == 'T') && (n_A != n_B || m_A != m_C || m_B != n_C)) return krr("dimension_mismatch");
    if ((transA == 'T' && transB == 'T') && (m_A != n_B || n_A != m_C || m_B != n_C)) return krr("dimension_mismatch");

    double* A_arr = k_to_double_array_column(A);
    double* B_arr = k_to_double_array_column(B);
    double* C_arr = k_to_double_array_column(C);

    if (!A_arr || !B_arr || !C_arr) {
        free(A_arr); free(B_arr); free(C_arr);
        return krr("memory");
    }

    int64_t lda = (transA == 'N') ? m_A : n_A;
    int64_t ldb = (transB == 'N') ? m_B : n_B;
    int64_t ldc = m_C;

    dgemm_(&transA, &transB, &m_A, &n_B, &n_A, &alpha, A_arr, &lda, B_arr, &ldb, &beta, C_arr, &ldc);

    K result = double_array_to_k_matrix(C_arr, m_C, n_C);
    free(A_arr); free(B_arr); free(C_arr);
    return result;
}

// Symmetric matrix-matrix multiplication -> C = alpha*A*B + beta*C or C = alpha*B*A + beta*C
ZK z_dsymm(K A, K B, K C, K params) {
    P(A->t != 0 || ((A->t == 0) && (kK(A)[0]->t != KF)), krr("type"));
    P(B->t != 0 || ((B->t == 0) && (kK(B)[0]->t != KF)), krr("type"));
    P(C->t != 0 || ((C->t == 0) && (kK(C)[0]->t != KF)), krr("type"));
    P(params->t != XD, krr("params_type"));

    double alpha = get_double_value(params, "alpha", 1.0);
    double beta = get_double_value(params, "beta", 1.0);
    char side = get_char_value(params, "side", 'L');
    char uplo = get_char_value(params, "uplo", 'U');

    int64_t m_A = A->n;
    int64_t n_A = kK(A)[0]->n;
    int64_t m_B = B->n;
    int64_t n_B = kK(B)[0]->n;
    int64_t m_C = C->n;
    int64_t n_C = kK(C)[0]->n;

    if (side == 'L') {
        if (m_A != n_A || m_A != m_B || n_B != n_C) return krr("dimension_mismatch");
    } else if (side == 'R') {
        if (m_A != n_A || n_A != n_B || m_B != m_C) return krr("dimension_mismatch");
    } else {
        return krr("invalid_side");
    }

    double* A_arr = k_to_double_array_column(A);
    double* B_arr = k_to_double_array_column(B);
    double* C_arr = k_to_double_array_column(C);

    if (!A_arr || !B_arr || !C_arr) {
        free(A_arr); free(B_arr); free(C_arr);
        return krr("memory");
    }

    int64_t lda = m_A;
    int64_t ldb = m_B;
    int64_t ldc = m_C;

    dsymm_(&side, &uplo, &m_B, &n_B, &alpha, A_arr, &lda, B_arr, &ldb, &beta, C_arr, &ldc);

    K result = double_array_to_k_matrix(C_arr, m_C, n_C);
    free(A_arr); free(B_arr); free(C_arr);
    return result;
}

// Symmetric rank-k update -> C = alpha*A*A^T + beta*C or C = alpha*A^T*A + beta*C
ZK z_dsyrk(K A, K C, K params) {
    P(A->t != 0 || ((A->t == 0) && (kK(A)[0]->t != KF)), krr("type"));
    P(C->t != 0 || ((C->t == 0) && (kK(C)[0]->t != KF)), krr("type"));
    P(params->t != XD, krr("params_type"));

    double alpha = get_double_value(params, "alpha", 1.0);
    double beta = get_double_value(params, "beta", 1.0);
    char uplo = get_char_value(params, "uplo", 'U');
    char trans = get_char_value(params, "trans", 'N');

    int64_t m_A = A->n;
    int64_t n_A = kK(A)[0]->n;
    int64_t m_C = C->n;
    int64_t n_C = kK(C)[0]->n;

    if (trans == 'N') {
        if (m_C != n_C || m_C != m_A) return krr("dimension_mismatch");
    } else if (trans == 'T') {
        if (m_C != n_C || m_C != n_A) return krr("dimension_mismatch");
    } else {
        return krr("invalid_trans");
    }

    double* A_arr = k_to_double_array_column(A);
    double* C_arr = k_to_double_array_column(C);

    if (!A_arr || !C_arr) {
        free(A_arr); free(C_arr);
        return krr("memory");
    }

    int64_t lda = (trans == 'N') ? m_A : n_A;
    int64_t ldc = m_C;
    int64_t k = (trans == 'N') ? n_A : m_A;

    dsyrk_(&uplo, &trans, &m_C, &k, &alpha, A_arr, &lda, &beta, C_arr, &ldc);

    K result = double_array_to_k_matrix(C_arr, m_C, n_C);
    free(A_arr); free(C_arr);
    return result;
}

// Symmetric rank-2k update -> C = alpha*A*B' + alpha*B*A' + beta*C
ZK z_dsyr2k(K A, K B, K C, K params) {
    P(A->t != 0 || ((A->t == 0) && (kK(A)[0]->t != KF)), krr("type"));
    P(B->t != 0 || ((B->t == 0) && (kK(B)[0]->t != KF)), krr("type"));
    P(C->t != 0 || ((C->t == 0) && (kK(C)[0]->t != KF)), krr("type"));
    P(params->t != XD, krr("params_type"));

    double alpha = get_double_value(params, "alpha", 1.0);
    double beta = get_double_value(params, "beta", 1.0);
    char uplo = get_char_value(params, "uplo", 'U');
    char trans = get_char_value(params, "trans", 'N');

    int64_t m_A = A->n;
    int64_t n_A = kK(A)[0]->n;
    int64_t m_B = B->n;
    int64_t n_B = kK(B)[0]->n;
    int64_t m_C = C->n;
    int64_t n_C = kK(C)[0]->n;

    if (trans == 'N') {
        if (m_A != m_B || n_A != n_B || m_C != n_C || m_C != m_A) return krr("dimension_mismatch");
    } else if (trans == 'T') {
        if (m_A != m_B || n_A != n_B || m_C != n_C || m_C != n_A) return krr("dimension_mismatch");
    } else {
        return krr("invalid_trans");
    }

    double* A_arr = k_to_double_array_column(A);
    double* B_arr = k_to_double_array_column(B);
    double* C_arr = k_to_double_array_column(C);

    if (!A_arr || !B_arr || !C_arr) {
        free(A_arr); free(B_arr); free(C_arr);
        return krr("memory");
    }

    int64_t lda = (trans == 'N') ? m_A : n_A;
    int64_t ldb = (trans == 'N') ? m_B : n_B;
    int64_t ldc = m_C;
    int64_t k = (trans == 'N') ? n_A : m_A;

    dsyr2k_(&uplo, &trans, &m_C, &k, &alpha, A_arr, &lda, B_arr, &ldb, &beta, C_arr, &ldc);

    K result = double_array_to_k_matrix(C_arr, m_C, n_C);
    free(A_arr); free(B_arr); free(C_arr);
    return result;
}

// Triangular matrix-matrix multiplication -> B = alpha*op(A)*B or B = alpha*B*op(A)
ZK z_dtrmm(K A, K B, K params) {
    P(A->t != 0 || ((A->t == 0) && (kK(A)[0]->t != KF)), krr("type"));
    P(B->t != 0 || ((B->t == 0) && (kK(B)[0]->t != KF)), krr("type"));
    P(params->t != XD, krr("params_type"));

    double alpha = get_double_value(params, "alpha", 1.0);
    char side = get_char_value(params, "side", 'L');
    char uplo = get_char_value(params, "uplo", 'U');
    char transA = get_char_value(params, "transA", 'N');
    char diag = get_char_value(params, "diag", 'N');

    int64_t m_A = A->n;
    int64_t n_A = kK(A)[0]->n;
    int64_t m_B = B->n;
    int64_t n_B = kK(B)[0]->n;

    if (side == 'L') {
        if (m_A != n_A || m_A != m_B) return krr("dimension_mismatch");
    } else if (side == 'R') {
        if (m_A != n_A || m_A != n_B) return krr("dimension_mismatch");
    } else {
        return krr("invalid_side");
    }

    double* A_arr = k_to_double_array_column(A);
    double* B_arr = k_to_double_array_column(B);

    if (!A_arr || !B_arr) {
        free(A_arr); free(B_arr);
        return krr("memory");
    }

    int64_t lda = m_A;
    int64_t ldb = m_B;

    dtrmm_(&side, &uplo, &transA, &diag, &m_B, &n_B, &alpha, A_arr, &lda, B_arr, &ldb);

    K result = double_array_to_k_matrix(B_arr, m_B, n_B);
    free(A_arr); free(B_arr);
    return result;
}

// Solving triangular matrix equations -> op(A)*X = alpha*B or X*op(A) = alpha*B
ZK z_dtrsm(K A, K B, K params) {
    P(A->t != 0 || ((A->t == 0) && (kK(A)[0]->t != KF)), krr("type"));
    P(B->t != 0 || ((B->t == 0) && (kK(B)[0]->t != KF)), krr("type"));
    P(params->t != XD, krr("params_type"));

    double alpha = get_double_value(params, "alpha", 1.0);
    char side = get_char_value(params, "side", 'L');
    char uplo = get_char_value(params, "uplo", 'U');
    char transA = get_char_value(params, "transA", 'N');
    char diag = get_char_value(params, "diag", 'N');

    int64_t m_A = A->n;
    int64_t n_A = kK(A)[0]->n;
    int64_t m_B = B->n;
    int64_t n_B = kK(B)[0]->n;

    if (side == 'L') {
        if (m_A != n_A || m_A != m_B) return krr("dimension_mismatch");
    } else if (side == 'R') {
        if (m_A != n_A || m_A != n_B) return krr("dimension_mismatch");
    } else {
        return krr("invalid_side");
    }

    double* A_arr = k_to_double_array_column(A);
    double* B_arr = k_to_double_array_column(B);

    if (!A_arr || !B_arr) {
        free(A_arr); free(B_arr);
        return krr("memory");
    }

    int64_t lda = m_A;
    int64_t ldb = m_B;

    dtrsm_(&side, &uplo, &transA, &diag, &m_B, &n_B, &alpha, A_arr, &lda, B_arr, &ldb);

    K result = double_array_to_k_matrix(B_arr, m_B, n_B);
    free(A_arr); free(B_arr);
    return result;
}

typedef struct {
    char* name;
    char* filename;
} BLAS_LIBRARY;

BLAS_LIBRARY lib_defaults[] = {
    {"appl", "/System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/vecLib"},
    {"mkl", "libmkl_rt" F_W_U(".dll", ".so")},
    {"netlib", "libblas" F_W_U(".dll", ".so.3")},
    {"openblas", "libopenblas_64" F_W_U(".dll", ".so.0")}
};

Z BLAS_LIBRARY* find_lib(S s) {
    DO(sizeof(lib_defaults)/sizeof(*lib_defaults),P(!strcasecmp(s,lib_defaults[i].name),lib_defaults+i))R 0;
}

F_REG {
    P(xn<=1,krr("length"))
    P(xK[1]->t!=XD,krr("type"))
    K d = xK[1];
    K v0 = k(0,"{x in key y}",ks("lib"),r1(d),0);
    P(!v0->g,(r0(v0),krr("key")))
    r0(v0);
    K v1 = k(0,"@",r1(d),ks("lib"),0);
    char library_filename[512];
    K v2 = !v1->t && v1->n>0 ? kK(v1)[0] : v1;
    if(v2->t==KC) {
        memcpy(library_filename,kG(v2),v2->n);
        library_filename[v2->n]='\0';
    } else if (v2->t==KS || v2->t==-KS) {
        S s = v2->t==KS ? kS(v2)[0] : v2->s;
        BLAS_LIBRARY *bl = find_lib(s);
        if(!bl) R(r0(v1),krr("value"));
        strcpy(library_filename, bl->filename);
    } else R(r0(v1),krr("value"));
    r0(v1);
    void* h = NULL;
    h = fusion_lib(library_filename);
    P(!h,krr("Library load failed"))
   
    // Load all function symbols
    #define _(r, n, a) \
        if (!(n = fusion_sym(h, #n))) { \
            fprintf(stderr, "Error: Failed to load symbol '%s' from %s\n", #n, library_filename); \
            return krr(fusion_err()); \
        }
    FNS_L1
    FNS_L2
    FNS_L3
    #undef _

    printf("Successfully loaded BLAS library: %s\n", library_filename);

    K n = ktn(KS, 0), f = ktn(0, 0);
    // Level 1 BLAS functions
    #define _(s,a) js(&n,ss(#s));jk(&f,dl(z_##s,a));
    _(dasum,1)_(daxpy,3)_(dcopy,2)_(ddot,2)_(dnrm2,1)_(drot,4)_(drotg,2)_(drotm,3)_(drotmg,4)_(dscal,2)_(dswap,2)_(idamax,1)
    // Level 2 BLAS functions
    _(dgemv,4)_(dgbmv,4)_(dsymv,4)_(dsbmv,4)_(dspmv,4)_(dtrmv,3)_(dger,4)
    // Level 3 BLAS functions
    _(dgemm,4)_(dsymm,4)_(dsyrk,3)_(dsyr2k,4)_(dtrmm,3)_(dtrsm,3)

    return xD(n, f);
}