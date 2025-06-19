\\ (1) A(x) = exp(5*x*A(x)^(1/5)).
my(A=1, n=22); for(i=1, n, A = exp(5*x*A^(1/5) + x*O(x^n)) ); Vec(serlaplace(A))

\\ (2) A(x) = 1/A(-x*A(x)^(2/5)).
my(A=1, n=22); for(i=1, n, A = exp(5*x*A^(1/5) + x*O(x^n)) ); Vec(serlaplace(1/subst(A, x, -x*A^(2/5))))




