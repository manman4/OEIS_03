M=30;
\\ E.g.f. A(x) satisfies A(x) = 1 + (exp((x * A(x))^4) - 1)/x.
my(A=1, N=M); for(k=1, N, A=1+(exp((x*A)^4)-1)/x+x*O(x^N)); Vec(serlaplace(A))
