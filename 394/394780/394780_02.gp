M=20;

\\ G.f. A(x) satisfies A(x) = 1 - x*A(x)^2*(1 - 2*A(x)) + x^2*d/dx(A(x)^2).
my(A=1, n=M); for(i=1, n, A=1 - x*A^2*(1 - 2*A) + x^2*deriv(A^2) + x*O(x^M)); Vec(A)