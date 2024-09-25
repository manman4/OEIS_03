M=19;

\\ E.g.f. satisfies A(x) = exp(x^2 * (1 + x) * A(x)^2).
my(A=1, n=M); for(i=1, n, A=exp(x^2 * (1+x) * A^2 + x*O(x^n))); Vec(serlaplace(A))

