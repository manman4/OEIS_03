M=31;

\\ G.f. A(x) satisfies A(x) = 1/(1 - x*A(x)/(1 - x^3)).
my(A=1, n=M); for(i=1, n, A=1/(1 - x*A/(1 - x^3)) + x*O(x^n)); Vec(A)



