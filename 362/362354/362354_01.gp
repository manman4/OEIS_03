\\ (1) A(x) = exp(3*x*A(x)^(1/3)).
my(A=1, n=22); for(i=1, n, A = exp(3*x*A^(1/3) + x*O(x^n)) ); Vec(serlaplace(A))

\\ (2) A(x) = 1/A(-x*A(x)^(2/3)).
my(A=1, n=22); for(i=1, n, A = exp(3*x*A^(1/3) + x*O(x^n)) ); Vec(serlaplace(1/subst(A, x, -x*A^(2/3))))




