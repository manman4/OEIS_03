\\ (1) A(x) = exp(2*x*A(x)^(5/2))
my(A=1, n=22); for(i=1, n, A = exp(2*x*A^(5/2) + x*O(x^n)) ); Vec(serlaplace(A))

\\ (2) A(x) = 1/A(-x*A(x)^5).
my(A=1, n=22); for(i=1, n, A = exp(2*x*A^(5/2) + x*O(x^n)) ); Vec(serlaplace(1/subst(A, x, -x*A^5)))




