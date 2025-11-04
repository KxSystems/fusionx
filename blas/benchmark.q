\l f.q
lib:$[0<count .z.x;first .z.x;"libopenblas.so"]
if[not any lib like/: ("*so*";"*dll");lib:`$lib]

openblas: fusion(`blas;([lib:lib]))
-1 "LEVEL 3";

// random matrices (row by row to avoid size issue on my system)
randMat: {[m;n] flip m#enlist n?1.0 }

upperTriangular: {[mat]
    n: count mat;
    mask: ((til n) >=/: til n);
    mat * mask
 }

largeM: 5000
largeN: 5000
largeK: 5000

A_large: randMat[largeM; largeK]
B_large: randMat[largeK; largeN]
C_large: randMat[largeM; largeN]

alpha_large: 1.5
beta_large: 0.5
transA_large: "N"
transB_large: "N"

-1 "Benchmarking DGEMM...";
params_dgemm: `alpha`beta`transA`transB!((alpha_large; beta_large; transA_large; transB_large))

// Time the Q implementation
start_time: .z.P
expected_dgemm: (alpha_large * A_large mmu B_large) + (beta_large * C_large)
end_time: .z.P
time_q_dgemm: end_time - start_time

// Time the OpenBLAS implementation
start_time: .z.P
result_dgemm: openblas.dgemm[A_large; B_large; C_large; params_dgemm]
end_time: .z.P
time_openblas_dgemm: end_time - start_time

speedup_dgemm: time_q_dgemm % time_openblas_dgemm

-1 "DGEMM - Q Time (ms): ", string time_q_dgemm;
-1 "DGEMM - OpenBLAS Time (ms): ", string time_openblas_dgemm;
-1 ssr["DGEMM - OpenBLAS is %dx faster than Q";"%d";-27!(2i;speedup_dgemm)];

-1 "\nBenchmarking DSYMM...";
side_large: "L"
uplo_large: "U"
params_dsymm: `alpha`beta`side`uplo!((alpha_large; beta_large; side_large; uplo_large))

start_time: .z.P
expected_dsymm: (alpha_large * A_large mmu B_large) + (beta_large * C_large)
end_time: .z.P
time_q_dsymm: end_time - start_time

start_time: .z.P
result_dsymm: openblas.dsymm[A_large; B_large; C_large; params_dsymm]
end_time: .z.P
time_openblas_dsymm: end_time - start_time

speedup_dsymm: time_q_dsymm % time_openblas_dsymm

-1 "DSYMM - Q Time (ms): ",string time_q_dsymm;
-1 "DSYMM - OpenBLAS Time (ms): ",string time_openblas_dsymm;
-1 ssr["DSYMM - OpenBLAS is %dx faster than Q";"%d";-27!(2i;speedup_dsymm)];

-1 "\nBenchmarking DSYRK...";
uplo_large: "U"
trans_large: "N"
params_dsyrk: `alpha`beta`uplo`trans!((alpha_large; beta_large; uplo_large; trans_large))

start_time: .z.P
expected_dsyrk: (alpha_large * A_large mmu flip A_large) + (beta_large * C_large)
end_time: .z.P
time_q_dsyrk: end_time - start_time

start_time: .z.P
result_dsyrk: openblas.dsyrk[A_large; C_large; params_dsyrk]
end_time: .z.P
time_openblas_dsyrk: end_time - start_time

speedup_dsyrk: time_q_dsyrk % time_openblas_dsyrk

-1 "DSYRK - Q Time (ms): ",string time_q_dsyrk;
-1 "DSYRK - OpenBLAS Time (ms): ",string time_openblas_dsyrk;
-1 ssr["DSYRK - OpenBLAS is %dx faster than Q";"%d";-27!(2i;speedup_dsyrk)];

-1 "\nBenchmarking DSYR2K...";
B_large_dsyr2k: randMat[largeM; largeN]
params_dsyr2k: `alpha`beta`uplo`trans!((alpha_large; beta_large; uplo_large; trans_large))

start_time: .z.P
expected_dsyr2k: (alpha_large * ((A_large mmu flip B_large_dsyr2k) + (B_large_dsyr2k mmu flip A_large))) + (beta_large * C_large)
end_time: .z.P
time_q_dsyr2k: end_time - start_time

start_time: .z.P
result_dsyr2k: openblas.dsyr2k[A_large; B_large_dsyr2k; C_large; params_dsyr2k]
end_time: .z.P
time_openblas_dsyr2k: end_time - start_time

speedup_dsyr2k: time_q_dsyr2k % time_openblas_dsyr2k

-1 "DSYR2K - Q Time (ms): ",string time_q_dsyr2k;
-1 "DSYR2K - OpenBLAS Time (ms): ",string time_openblas_dsyr2k;
-1 ssr["DSYR2K - OpenBLAS is %dx faster than Q";"%d";-27!(2i;speedup_dsyr2k)];

-1 "\nBenchmarking DTRMM...";
side_large: "L"
uplo_large: "U"
transA_large: "N"
diag_large: "N"
params_dtrmm: `alpha`side`uplo`transA`diag!((alpha_large; side_large; uplo_large; transA_large; diag_large))

start_time: .z.P
expected_dtrmm: alpha_large * A_large mmu B_large
end_time: .z.P
time_q_dtrmm: end_time - start_time

start_time: .z.P
result_dtrmm: openblas.dtrmm[A_large; B_large; params_dtrmm]
end_time: .z.P
time_openblas_dtrmm: end_time - start_time

speedup_dtrmm: time_q_dtrmm % time_openblas_dtrmm

-1 "DTRMM - Q Time (ms): ",string time_q_dtrmm;
-1 "DTRMM - OpenBLAS Time (ms): ",string time_openblas_dtrmm;
-1 ssr["DTRMM - OpenBLAS is %dx faster than Q";"%d";-27!(2i;speedup_dtrmm)];

-1 "\nBenchmarking DTRSM...";
solveTrsm: {[A;B]
    m: count B;
    n: count first B;
    X: B;
    X[1]: B[1]%A[1;1];
    X[0]: B[0]-A[0;1]*X[1];
    X
 }
params_dtrsm: `alpha`side`uplo`transA`diag!((alpha_large; side_large; uplo_large; transA_large; diag_large))

start_time: .z.P
expected_dtrsm: solveTrsm[A_large; B_large]
end_time: .z.P
time_q_dtrsm: end_time - start_time

start_time: .z.P
result_dtrsm: openblas.dtrsm[A_large; B_large; params_dtrsm]
end_time: .z.P
time_openblas_dtrsm: end_time - start_time

speedup_dtrsm: time_q_dtrsm % time_openblas_dtrsm

-1 "DTRSM - Q Time (ms): ",string time_q_dtrsm;
-1 "DTRSM - OpenBLAS Time (ms): ",string time_openblas_dtrsm;
-1 ssr["DTRSM - OpenBLAS is %dx faster than Q";"%d";-27!(2i;speedup_dtrsm)];
\\