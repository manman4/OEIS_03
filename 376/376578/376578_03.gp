M=31;

\\ E.g.f. A(x) satisfies A(x) = exp(x*(1 + x^3)*A(x)).
my(A=1, n=M); for(i=1, n, A=exp(x*(1 + x^3)*A) + x*O(x^n)); Vec(serlaplace(A))


