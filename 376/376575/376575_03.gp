M=31;

\\ E.g.f. A(x) satisfies A(x) = exp(x*A(x)/(1 - x^2)).
my(A=1, n=M); for(i=1, n, A=exp(x*A/(1 - x^2)) + x*O(x^n)); Vec(serlaplace(A))


