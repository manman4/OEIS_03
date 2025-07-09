M=19;

\\ G.f. A(x) satisfies A(x) = 1/((1 - x) * (1 - x*A(x)^2 - x^2*A(x)*A'(x))).
my(A=1, n=M); for(i=1, n, A= 1/((1 - x) * (1 - x*A^2 - x^2*A*deriv(A) + x*O(x^n)) )); Vec(A)

