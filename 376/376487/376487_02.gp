M=49;

\\ G.f. satisfies A(x) = 1 / (1 - x^4*A(x)^4 * (1 + x)).
my(A=1, n=M); for(i=1, n, A=1/(1-x^4*A^4*(1+x) + x*O(x^n))); Vec(A)


