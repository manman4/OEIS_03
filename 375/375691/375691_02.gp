M=41;

\\ G.f. A(x) satisfies A(x) = 1 + x^4*(1+x)*A(x)^3.
my(A=1, n=M); for(i=1, n, A=1 + x^4*(1+x)*A^3 + x*O(x^n)); Vec(A)


