M=20;

\\ E.g.f. satisfies A(x) = exp(x^3 * A(x) / (1 - x)).
my(A=1, n=M); for(i=1, n, A=exp(x^3 * A / (1-x + x*O(x^n)))); Vec(serlaplace(A))


