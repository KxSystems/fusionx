blas:use(`$"fusion:",string M;([lib:B]));
tol:1e-5;
n:1000000;

// LEVEL 1 TESTS

// Test dasum
t)abs[blas.dasum[(n#1.0)+(n#til n)%100000]-sum (n#1.0)+(n#til n)%100000]<tol

// Test daxpy
x: (n#1.0) + (n#til n)%10000
y: (n#1.0) - (n#til n)%10000
alpha: 2.5
daxpy: { [x; y; alpha] y + alpha * x }
expected: daxpy[x; y; alpha]
result: blas.daxpy[x; y; alpha]
diff: abs[result - expected]
t)all diff<tol



// Test dcopy
x:n#1.0+(n#til n)%10000;
y:n#0.0;
expected: x
result: blas.dcopy[x; y]
diff: abs[result - expected]
t)all diff<tol

x: n#1.0 + (n#til n)%10000
y: n#0.0
expected: x
result: blas.dcopy[x; y]
diff: abs[result - expected]
t)all diff<tol


// Test ddot
x: (n#1.0) + (n#til n)%1000
y: (n#1.0) - (n#til n)%1000
expected: sum x * y
result: blas.ddot[x; y]
diff: abs[result - expected]
/Tolerance need adjustment for ddot due to OpenBLAS accumulation strategy
ddot_tol:1e-3
t)diff < ddot_tol




// Test dnrm2
x: (n#1.0) + (n#til n)%1000
expected: sqrt sum x*x
result: blas.dnrm2[x]
diff: abs[result - expected]
t)diff<tol


// Test drot
x: (n#1.0) + (n#til n)%1000
y: (n#1.0) - (n#til n)%1000
c: cos 0.5
s: sin 0.5
expectedX: (c * x) + (s * y)
expectedY: (c * y) - (s * x)
blas.drot[x; y; c; s]
resultX: x
resultY: y

diffX: abs[resultX - expectedX]
diffY: abs[resultY - expectedY]
t)all diffX < tol and all diffY < tol


// Test drotg
a: 3.0
b: 4.0
expectedR: sqrt (a*a) + (b*b)  // 5.0
expectedC: a % expectedR       // 0.6
expectedS: b % expectedR       // 0.8
result: blas.drotg[a; b]
diffR: abs result[0] - expectedR
diffC: abs result[2] - expectedC
diffS: abs result[3] - expectedS
t)all (diffR;diffC;diffS) < tol


// Test drotm
x: 1 2 3f
y: 4 5 6f
param: -1 -1 -1 1 1f
expected_x: 3 3 3f
expected_y: 3 3 3f
result: blas.drotm[x; y; param]
x_updated: result[0]
y_updated: result[1]
tol: 1e-10
t)all abs[x_updated - expected_x] < tol and all abs[y_updated - expected_y] < tol

// Test drotmg
da: 1.0
db: 2.0
dx: 3.0
dy: 4.0
expected: 1 0.375 0 0 0.75f
result: blas.drotmg[da; db; dx; dy]
t)expected ~ result

// Test dscal
x: (n#1.0) + (n#til n)%1000
alpha: 3.5
expected: x * alpha
result: blas.dscal[alpha; x]
diff: abs[result - expected]
t)all diff < tol


// Test dswap
x: (n#1.0) + (n#til n)%1000
x2: (n#1.0) + (n#til n)%1000 /identical to x for comparison
y: (n#1.0) - (n#til n)%1000
y2: (n#1.0) - (n#til n)%1000 /identical to y for comparison
blas.dswap[x; y]  // Modifies x and y in place
diffX: abs[x - y2]
diffY: abs[y - x2]
t)all diffX=0 and all diffY=0


// Test idamax
x: (n#1.0) - (n#til n)%1000
expected: abs[x]?max abs[x]
result: blas.idamax[x]
t)result ~ expected

// LEVEL 2

/ // Test dgemv
n: 2
m: 2
matr: (n,m) # 1.0 2.0 3.0 4.0        /  [[1,2],[3,4]]
x: n#1.0 2.0                      / [1,2]
y: n#0.5 0.5                  / [0.5,0.5]
params: `alpha`beta`trans! (1.5f; 0.5f; "N")

/ Expected calculation:
/ 1.5 * (matr mmu x) + 0.5 * y
/ where matr mmu x = [1*1 + 2*2, 3*1 + 4*2]
step1: sum each matr * \: x          / A * x
step2: params[`alpha] * step1        / alpha * (A * x)
step3: params[`beta] * y             / beta * y
expected: step2 + step3                / Final result
result: blas.dgemv[matr; x; y; params]
tol: 1e-10
t)all abs[result - expected] < tol

// Test dgbmv
n: 2
m: 2
kl: 1
ku: 1

matr: (n,m) # 1.0 2.0 3.0 4.0 / [[1,2],[3,4]]
x: n#1.0 2.0 / [1,2]
y: n#0.5 0.5 / [0.5,0.5]
params: `alpha`beta`trans`kl`ku! (1.5f; 0.5f; "N"; kl; ku)

// DGEMV manual calculation
/ 1.5 * (matr mmu x) + 0.5 * y
/ matr mmu x = [1*1 + 2*2, 3*1 + 4*2]
step1: sum each matr *\: x
step2: params[`alpha] * step1
step3: params[`beta] * y
expected: step2 + step3

result: blas.dgbmv[matr; x; y; params]
tol: 1e-10
t)all abs[result - expected] < tol

// Test dsymv
n: 2
matr: (n,n) # 1.0 2.0 2.0 4.0        / Symmetric matrix [[1,2],[2,4]]
x: n#1.0 2.0                      / [1,2]
y: n#0.5 0.5                  / [0.5,0.5]
params: `alpha`beta`uplo! (1.5f; 0.5f; "U")  / Upper triangular part used
/ Expected calculation:
/ 1.5 * (matr mmu x) + 0.5 * y
/ where matr mmu x = [1*1 + 2*2, 2*1 + 4*2]
step1: sum each matr *\: x          / A * x
step2: params[`alpha] * step1        / alpha * (A * x)
step3: params[`beta] * y             / beta * y
expected: step2 + step3                / Final result
result: blas.dsymv[matr; x; y; params]
tol: 1e-10
t)all abs[result - expected] < tol

// Test dsbmv
n: 2
ku: 1                               / Bandwidth (number of superdiagonals)
matr: (n,n) # 1.0 2.0 2.0 4.0        / Symmetric banded matrix [[1,2],[2,4]]
x: n#1.0 2.0                      / [1,2]
y: n#0.5 0.5                  / [0.5,0.5]
params: `alpha`beta`uplo`ku! (1.5f; 0.5f; "U"; ku)  / Upper triangular part used
/ Expected calculation:
/ 1.5 * (matr mmu x) + 0.5 * y
/ where matr mmu x = [1*1 + 2*2, 2*1 + 4*2]
step1: sum each matr *\: x          / A * x
step2: params[`alpha] * step1        / alpha * (A * x)
step3: params[`beta] * y             / beta * y
expected: step2 + step3                / Final result
result: blas.dsbmv[matr; x; y; params]
tol: 1e-10
t)all abs[result - expected] < tol

// Test dspmv
n: 2
matr: 1.0 2.0 4.0                   / Packed symmetric matrix [1,2,4]
x: n#1.0 2.0                      / [1,2]
y: n#0.5 0.5                  / [0.5,0.5]
params: `alpha`beta`uplo! (1.5f; 0.5f; "U")  / Upper triangular part used
/ Expected calculation:
/ 1.5 * (matr mmu x) + 0.5 * y
/ where matr mmu x = [1*1 + 2*2, 2*1 + 4*2]
step1: sum each ((n,n) # 1.0 2.0 2.0 4.0) *\: x  / A * x
step2: params[`alpha] * step1        / alpha * (A * x)
step3: params[`beta] * y             / beta * y
expected: step2 + step3                / Final result
result: blas.dspmv[matr; x; y; params]
tol: 1e-10
t)all abs[result - expected] < tol

// Test dger
m: 2
n: 2
matr: (m,n) # 1.0 2.0 3.0 4.0        / Initial matrix [[1,2],[3,4]]
x: m#1.0 2.0                      / [1,2]
y: n#0.5 0.5                  / [0.5,0.5]
alpha: 1.5f                       / Scalar multiplier
/ Expected calculation:
/ alpha * x outer y = 1.5 * [[1*0.5, 1*0.5], [2*0.5, 2*0.5]] = [[0.75, 0.75], [1.5, 1.5]]
outer: {x*\:y}
rank1: alpha * outer[x;y]
expected: matr + rank1               / Final result
result: blas.dger[matr; x; y; alpha]
t)expected ~ result

// Test dtrmv
n: 2
matr: (n,n) # 1.0 2.0 0.0 4.0        / Upper triangular matrix [[1,2],[0,4]]
x: n#1.0 2.0                      / [1,2]
params: `uplo`trans`diag`dummy! ("U"; "N"; "N"; 0.1)  / Upper triangular, no transpose, non-unit diagonal
/ Expected calculation:
/ matr mmu x = [1*1 + 2*2, 0*1 + 4*2]
step1: sum each matr *\: x          / A * x
expected: step1                        / Final result
result: blas.dtrmv[matr; x; params]
tol: 1e-10
t)all abs[result - expected] < tol


// LEVEL 3


upperTriangular: {[mat]
    n: count mat;
    mask: ((til n) >=/: til n);
    mat * mask
 }



// Test dgemm
m: 2
n: 2
k: 2
A: (m,k) # 1.0 2.0 3.0 4.0    / 2x2 matrix A: [[1,2], [3,4]]
B: (k,n) # 5.0 6.0 7.0 8.0    / 2x2 matrix B: [[5,6], [7,8]]
C: (m,n) # 0.5 0.5 0.5 0.5    / 2x2 matrix C: [[0.5,0.5], [0.5,0.5]]
alpha: 1.5f                   / Scalar multiplier for A*B
beta: 0.5f                    / Scalar multiplier for C
transA: "N"                   / No transpose for A
transB: "N"                   / No transpose for B
/ Expected calculation:
/ alpha * (A . B) + beta * C
expected: (alpha * A mmu B) + (beta * C)
params: `alpha`beta`transA`transB!((alpha; beta; transA; transB))
result: blas.dgemm[A; B; C; params]
t)expected ~ result

// Test dsymm
m: 2
n: 3
A: (m,m) # 1.0 2.0 2.0 3.0  / [[1, 2], [2, 3]]
B: (m,n) # 1.0 2.0 3.0 4.0 5.0 6.0  / [[1, 2, 3], [4, 5, 6]]
C: (m,n) # 0.5 0.5 0.5 0.5 0.5 0.5  / [[0.5, 0.5, 0.5], [0.5, 0.5, 0.5]]
alpha: 1.5
beta: 0.5
side: "L"  / Left multiplication
uplo: "U"  / Use upper triangular part of A
/ Expected calculation:
/ C = alpha * A * B + beta * C
expected: (alpha * A mmu B) + (beta * C)
params: `alpha`beta`side`uplo!((alpha; beta; side; uplo))
result: blas.dsymm[A; B; C; params]
t)expected ~ result

// Test dsyrk
m: 2
n: 3
A: (m,n) # 1.0 2.0 3.0 4.0 5.0 6.0
C: (m,m) # 0.5 0.5 0.5 0.5
alpha: 1.5
beta: 0.5
uplo: "U"
trans: "N"
/ Expected calculation:
/ C = alpha * A * A^T + beta * C
expected: (alpha * A mmu flip A) + (beta * C)
params: `alpha`beta`uplo`trans!((alpha; beta; uplo; trans))
result: blas.dsyrk[A; C; params]

upperResult: upperTriangular[result]
upperExpected: upperTriangular[expected]
t)upperResult ~ upperExpected


// Test dsyr2k
m: 2
n: 3
A: (m,n) # 1.0 2.0 3.0 4.0 5.0 6.0  / [[1, 2, 3], [4, 5, 6]]
B: (m,n) # 7.0 8.0 9.0 10.0 11.0 12.0  / [[7, 8, 9], [10, 11, 12]]
C: (m,m) # 0.5 0.5 0.5 0.5  / [[0.5, 0.5], [0.5, 0.5]]
alpha: 1.5
beta: 0.5
uplo: "U"  / Use upper triangular part of C
trans: "N" / No transpose of A and B
/ Expected calculation:
/ C = alpha * (A * B^T + B * A^T) + beta * C
expected: (alpha * ((A mmu flip B) + (B mmu flip A))) + (beta * C)
params: `alpha`beta`uplo`trans!((alpha; beta; uplo; trans))
result: blas.dsyr2k[A; B; C; params]
upperResult: upperTriangular[result]
upperExpected: upperTriangular[expected]
t)upperResult ~ upperExpected

// Test dtrmm
m: 2
n: 3
A: (m,m) # 1.0 2.0 0.0 3.0  / [[1, 2], [0, 3]]
B: (m,n) # 1.0 2.0 3.0 4.0 5.0 6.0  / [[1, 2, 3], [4, 5, 6]]
alpha: 1.5
side: "L"   / Left multiplication
uplo: "U"   / Use upper triangular part of A
transA: "N" / No transpose of A
diag: "N"   / Non-unit diagonal
/ Expected calculation:
/ B = alpha * A * B
expected: alpha * A mmu B
params: `alpha`side`uplo`transA`diag!((alpha; side; uplo; transA; diag))
result: blas.dtrmm[A; B; params]
t)expected ~ result

// Test dtrsm
m: 2
n: 3
A: (m,m) # 1.0 2.0 0.0 3.0
B: (m,n) # 1.0 2.0 3.0 4.0 5.0 6.0
/ Parameters
alpha: 1.0
side: "L"
uplo: "U"
transA: "N"
diag: "N"
/ Expected calculation:
/ Solve A * X = B for X
solveTrsm:{[A;B]
    m:count B;
    n:count first B;
    X:B;
    X[1]:B[1]%A[1;1];
    X[0]:B[0]-A[0;1]*X[1];
    X
 };
expected:solveTrsm[A;B];
params: `alpha`side`uplo`transA`diag!((alpha; side; uplo; transA; diag))
result: blas.dtrsm[A; B; params]
t)expected ~ result