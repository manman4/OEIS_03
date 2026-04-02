M=20;

\\ G.f. A(x) satisfies A(x) = 1 - x*A(x) + x*A(x)^2 + x^4*A(x) + x^5*d/dx A(x).
my(A=1, n=M); for(i=1, n, A=1 - x*A + x*A^2 + x^4*A + x^5*deriv(A) + x*O(x^M)); Vec(A)
