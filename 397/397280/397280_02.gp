M=30;
\\ E.g.f. A(x) satisfies A(x) = 1 + (exp((x * A(x))^3) - 1)/x.
my(A=1, N=M); for(k=1, N, A=1+(exp((x*A)^3)-1)/x+x*O(x^N)); Vec(serlaplace(A))
