# BLAS Module API Reference

The KDB-X BLAS module provides q bindings to BLAS (Basic Linear Algebra Subprograms) functions for high-performance linear algebra operations.

For documentation purposes, examples assume the module has been loaded into the namespace `.blas`:

```q
q).blas:use(`fusion:blas;([lib:"libopenblas.so"]))
```

## Level 1 BLAS Functions

Level 1 BLAS functions perform vector-vector operations.

### dasum

*Sum of absolute values*

```q
.blas.dasum[x]
```

Where `x` is a float vector.

Returns the sum of absolute values: |x₁| + |x₂| + ... + |xₙ|

```q
q).blas.dasum[1 -2 3 -4f]
10f
```

### daxpy

*Vector addition with scalar multiplication*

```q
.blas.daxpy[x; y; alpha]
```

Where:
- `x` is a float vector
- `y` is a float vector (same length as x)
- `alpha` is a float scalar

Returns `y + alpha * x`

```q
q).blas.daxpy[1 2 3f; 4 5 6f; 2.0]
6 9 12f
```

### dcopy

*Copy vector*

```q
.blas.dcopy[x; y]
```

Where:
- `x` is a float vector
- `y` is a float vector (same length as x)

Returns a copy of `x`

```q
q).blas.dcopy[1 2 3f; 0 0 0f]
1 2 3f
```

### ddot

*Dot product*

```q
.blas.ddot[x; y]
```

Where:
- `x` is a float vector
- `y` is a float vector (same length as x)

Returns the dot product: x₁·y₁ + x₂·y₂ + ... + xₙ·yₙ

```q
q).blas.ddot[1 2 3f; 4 5 6f]
32f
```

### dnrm2

*Euclidean norm*

```q
.blas.dnrm2[x]
```

Where `x` is a float vector.

Returns the Euclidean norm: √(x₁² + x₂² + ... + xₙ²)

```q
q).blas.dnrm2[3 4f]
5f
```

### drot

*Apply plane rotation*

```q
.blas.drot[x; y; c; s]
```

Where:
- `x` is a float vector
- `y` is a float vector (same length as x)
- `c` is cosine of rotation angle (float)
- `s` is sine of rotation angle (float)

Applies plane rotation in place. Returns null. Modifies x and y such that:
- x ← c·x + s·y
- y ← c·y - s·x

```q
q)x:1 2 3f
q)y:4 5 6f
q)c: cos 0.5
q)s: sin 0.5
q)expectedX: (c * x) + (s * y)
q)expectedX
2.795285 4.152293 5.509301
q)expectedY: (c * y) - (s * x)
q)expectedY
3.030905 3.429062 3.827219
q).blas.drot[x; y; cos 0.5; sin 0.5]
q)x
2.795285 4.152293 5.509301
q)y
3.030905 3.429062 3.827219
```

### drotg

*Generate plane rotation*

```q
.blas.drotg[a; b]
```

Where `a` and `b` are float scalars.

Returns `(r; z; c; s)` where:
- r = √(a² + b²)
- z = Value of encoding used by BLAS
- c = cosine of rotation angle
- s = sine of rotation angle

```q
q).blas.drotg[3.0; 4.0]
5f
1.666667
0.6
0.8
```

### drotm

*Modified plane rotation*

```q
.blas.drotm[x; y; param]
```

Where:
- `x` is a float vector
- `y` is a float vector (same length as x)
- `param` is a 5-element float vector defining the modified rotation

Returns `(x_updated; y_updated)`

```q
q).blas.drotm[1 2 3f; 4 5 6f; -1 -1 -1 1 1f]
3 3 3f
3 3 3f
```

### drotmg

*Generate modified plane rotation*

```q
.blas.drotmg[d1; d2; x1; y1]
```

Where `d1`, `d2`, `x1`, `y1` are float scalars.

Returns a 5-element float vector of modified rotation parameters.

```q
q).blas.drotmg[1.0; 2.0; 3.0; 4.0]
1 0.375 0 0 0.75
```

### dscal

*Scale vector*

```q
.blas.dscal[alpha; x]
```

Where:
- `alpha` is a float scalar
- `x` is a float vector

Returns `alpha * x`

```q
q).blas.dscal[2.5; 1 2 3f]
2.5 5 7.5
```

### dswap

*Swap vectors*

```q
.blas.dswap[x; y]
```

Where:
- `x` is a float vector
- `y` is a float vector (same length as x)

Swaps x and y in place. Returns null.

```q
q)x:1 2 3f
q)y:4 5 6f
q).blas.dswap[x; y]
q)x
4 5 6f
q)y
1 2 3f
```

### idamax

*Index of maximum absolute value*

```q
.blas.idamax[x]
```

Where `x` is a float vector.

Returns the 0-based index of the element with maximum absolute value.

```q
q).blas.idamax[1 -5 3 2f]
1i
```

## Level 2 BLAS Functions

Level 2 BLAS functions perform matrix-vector operations.

### dgemv

*General matrix-vector multiplication*

```q
.blas.dgemv[A; x; y; params]
```

Where:
- `A` is a matrix (list of float vectors)
- `x` is a float vector
- `y` is a float vector
- `params` is a dictionary with keys:
  - `alpha` - scalar multiplier for A·x
  - `beta` - scalar multiplier for y
  - `trans` - transpose flag: "N" or "T"

Returns `alpha * op(A) * x + beta * y` where op(A) is A or Aᵀ

```q
q)A:(1 2f; 3 4f)
q)x:1 2f
q)y:0.5 0.5f
q).blas.dgemv[A; x; y; `alpha`beta`trans!(1.5; 0.5; "N")]
7.75 16.75
```

### dgbmv

*Banded matrix-vector multiplication*

```q
.blas.dgbmv[A; x; y; params]
```

Where:
- `A` is a matrix representing a banded matrix
- `x` is a float vector
- `y` is a float vector
- `params` is a dictionary with keys:
  - `alpha` - scalar multiplier
  - `beta` - scalar multiplier
  - `trans` - transpose flag: "N" or "T"
  - `kl` - number of subdiagonals
  - `ku` - number of superdiagonals

Returns `alpha * op(A) * x + beta * y` for banded matrix A where op(A) is A or Aᵀ

```q
q)A:(1 2f; 3 4f)
q)x:1 2f
q)y:0.5 0.5f
q).blas.dgbmv[A; x; y; `alpha`beta`trans`kl`ku!(1.5; 0.5; "N"; 1; 1)]
7.75 16.75
```

### dsymv

*Symmetric matrix-vector multiplication*

```q
.blas.dsymv[A; x; y; params]
```

Where:
- `A` is a symmetric matrix
- `x` is a float vector
- `y` is a float vector
- `params` is a dictionary with keys:
  - `alpha` - scalar multiplier
  - `beta` - scalar multiplier
  - `uplo` - "U" for upper or "L" for lower triangle

Returns `alpha * A * x + beta * y` where A is symmetric

```q
q)A:(1 2f; 2 4f)  / Symmetric matrix
q)x:1 2f
q)y:0.5 0.5f
q).blas.dsymv[A; x; y; `alpha`beta`uplo!(1.5; 0.5; "U")]
7.75 15.25
```

### dsbmv

*Symmetric banded matrix-vector multiplication*

```q
.blas.dsbmv[A; x; y; params]
```

Where:
- `A` is a symmetric banded matrix
- `x` is a float vector
- `y` is a float vector
- `params` is a dictionary with keys:
  - `alpha` - scalar multiplier
  - `beta` - scalar multiplier
  - `uplo` - "U" or "L"
  - `ku` - number of superdiagonals

Returns `alpha * A * x + beta * y` for symmetric banded matrix A

```q
q)A:(1 2 0f; 2 3 4f; 0 4 5f)  / Symmetric matrix
q)x:1 2 3f
q)y:0.1 0.2 0.3f
q).blas.dsbmv[A; x; y; `alpha`beta`uplo`ku!(2.0; 0.5; "U"; 1)]
10.05 40.1 46.15
```

### dspmv

*Symmetric packed matrix-vector multiplication*

```q
.blas.dspmv[A; x; y; params]
```

Where:
- `A` is a packed symmetric matrix (float vector)
- `x` is a float vector
- `y` is a float vector
- `params` is a dictionary with keys:
  - `alpha` - scalar multiplier
  - `beta` - scalar multiplier
  - `uplo` - "U" or "L"

Returns `alpha * A * x + beta * y` where A is stored in packed format

```q
q)A:1 2 4f  / Packed: [[1,2],[2,4]]
q)x:1 2f
q)y:0.5 0.5f
q).blas.dspmv[A; x; y; `alpha`beta`uplo!(1.5; 0.5; "U")]
7.75 15.25
```

### dger

*Rank-1 update of general matrix*

```q
.blas.dger[A; x; y; alpha]
```

Where:
- `A` is a matrix
- `x` is a float vector
- `y` is a float vector
- `alpha` is a float scalar

Returns `A + alpha * x * yᵀ` (outer product update)

```q
q)A:(1 2f; 3 4f)
q)x:1 2f
q)y:0.5 0.5f
q).blas.dger[A; x; y; 1.5]
1.75 2.75
4.5  5.5
```

### dtrmv

*Triangular matrix-vector multiplication*

```q
.blas.dtrmv[A; x; params]
```

Where:
- `A` is a triangular matrix
- `x` is a float vector
- `params` is a dictionary with keys:
  - `uplo` - "U" or "L"
  - `trans` - "N" or "T"
  - `diag` - "N" (non-unit) or "U" (unit diagonal)

Returns `op(A) * x` where A is triangular and op(A) is A or Aᵀ

```q
q)A:(1 2f; 0 4f)  / Upper triangular
q)x:1 2f
q).blas.dtrmv[A; x; `uplo`trans`diag`dummy!("U"; "N"; "N"; 0.1)]
5 8f
```

## Level 3 BLAS Functions

Level 3 BLAS functions perform matrix-matrix operations.

### dgemm

*General matrix-matrix multiplication*

```q
.blas.dgemm[A; B; C; params]
```

Where:
- `A` is a matrix
- `B` is a matrix
- `C` is a matrix
- `params` is a dictionary with keys:
  - `alpha` - scalar multiplier
  - `beta` - scalar multiplier
  - `transA` - "N" or "T"
  - `transB` - "N" or "T"

Returns `alpha * op(A) * op(B) + beta * C` where op(A) is A or Aᵀ

```q
q)A:(1 2f; 3 4f)
q)B:(5 6f; 7 8f)
q)C:(0.5 0.5f; 0.5 0.5f)
q).blas.dgemm[A; B; C; `alpha`beta`transA`transB!(1.5; 0.5; "N"; "N")]
28.75 33.25
64.75 75.25
```

### dsymm

*Symmetric matrix-matrix multiplication*

```q
.blas.dsymm[A; B; C; params]
```

Where:
- `A` is a symmetric matrix
- `B` is a matrix
- `C` is a matrix
- `params` is a dictionary with keys:
  - `alpha` - scalar multiplier
  - `beta` - scalar multiplier
  - `side` - "L" (A·B) or "R" (B·A)
  - `uplo` - "U" or "L"

Returns `alpha * A * B + beta * C` (if side="L")

```q
q)A:(1 2f; 2 3f)
q)B:(1 2 3f; 4 5 6f)
q)C:(0.5 0.5 0.5f; 0.5 0.5 0.5f)
q).blas.dsymm[A; B; C; `alpha`beta`side`uplo!(1.5; 0.5; "L"; "U")]
13.75 18.25 22.75
21.25 28.75 36.25
```

### dsyrk

*Symmetric rank-k update*

```q
.blas.dsyrk[A; C; params]
```

Where:
- `A` is a matrix
- `C` is a symmetric matrix
- `params` is a dictionary with keys:
  - `alpha` - scalar multiplier
  - `beta` - scalar multiplier
  - `uplo` - "U" or "L"
  - `trans` - "N" (A·Aᵀ) or "T" (Aᵀ·A)

Returns `alpha * op(A) * op(A)ᵀ + beta * C` where op(A) is A or Aᵀ

```q
q)A:(1 2 3f; 4 5 6f)
q)C:(0.5 0.5f; 0.5 0.5f)
q).blas.dsyrk[A; C; `alpha`beta`uplo`trans!(1.5; 0.5; "U"; "N")]
21.25 48.25
0.5   115.75
```

### dsyr2k

*Symmetric rank-2k update*

```q
.blas.dsyr2k[A; B; C; params]
```

Where:
- `A` is a matrix
- `B` is a matrix
- `C` is a symmetric matrix
- `params` is a dictionary with keys:
  - `alpha` - scalar multiplier
  - `beta` - scalar multiplier
  - `uplo` - "U" or "L"
  - `trans` - "N" or "T"

Returns `alpha * (A·Bᵀ + B·Aᵀ) + beta * C`

```q
q)A:(1 2 3f; 4 5 6f)
q)B:(7 8 9f; 10 11 12f)
q)C:(0.5 0.5f; 0.5 0.5f)
q).blas.dsyr2k[A; B; C; `alpha`beta`uplo`trans!(1.5; 0.5; "U"; "N")]
150.25 285.25
0.5    501.25
```

### dtrmm

*Triangular matrix-matrix multiplication*

```q
.blas.dtrmm[A; B; params]
```

Where:
- `A` is a triangular matrix
- `B` is a matrix
- `params` is a dictionary with keys:
  - `alpha` - scalar multiplier
  - `side` - "L" or "R"
  - `uplo` - "U" or "L"
  - `transA` - "N" or "T"
  - `diag` - "N" or "U"

Returns `alpha * op(A) * B` (if side="L") where op(A) is A or Aᵀ

```q
q)A:(1 2f; 0 3f)
q)B:(1 2 3f; 4 5 6f)
q).blas.dtrmm[A; B; `alpha`side`uplo`transA`diag!(1.5; "L"; "U"; "N"; "N")]
13.5 18 22.5
18   22.5 27
```

### dtrsm

*Solve triangular system*

```q
.blas.dtrsm[A; B; params]
```

Where:
- `A` is a triangular matrix
- `B` is a matrix
- `params` is a dictionary with keys:
  - `alpha` - scalar multiplier
  - `side` - "L" or "R"
  - `uplo` - "U" or "L"
  - `transA` - "N" or "T"
  - `diag` - "N" or "U"

Solves `op(A) * X = alpha * B` for X (if side="L")

```q
q)A:(1 2f; 0 3f)
q)B:(1 2 3f; 4 5 6f)
q).blas.dtrsm[A; B; `alpha`side`uplo`transA`diag!(1.0; "L"; "U"; "N"; "N")]
-1.666667 -1.333333 -1
1.333333  1.666667  2
```


## Error Handling

Common errors and their meanings:

-  `'type` - Incorrect argument type (expected float vector/matrix)
- `'length` - Vector/matrix dimension mismatch
- `'dimension_mismatch` - Matrix dimensions incompatible for operation
- `'memory` - Memory allocation failed
- `'matrix_type` - Expected matrix (list of float vectors)
- `'params_type` - Expected dictionary for params argument
