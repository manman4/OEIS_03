M=20;

\\ G.f. A(x) satisfies A(x) = 1 - 2*x * d/dx log(1 - x*A(x)^2).
my(A=1, n=M); for(i=1, n, A=1 - 2*x*deriv(log(1 - x*A^2 + x*O(x^M)))); Vec(A)