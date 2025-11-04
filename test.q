p:.Q.opt .z.x;
t:p[`test]
B:$[`blas in key p;first p[`blas];"libblas.so.3"]
if[not any B like/: ("*so*";"*dll");B:`$B]

.t.e:{if[not value x;-1 x]}
{M::`$first"/"vs x;system"l ",x}each t;
\\
