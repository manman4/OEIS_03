M=30;
\\ E.g.f. A(x) satisfies A(x) = 1 - log(1 - (x * A(x))^3)/x.
my(A=1, N=M); for(k=1, N, A=1-log(1-(x*A)^3)/x+x*O(x^N)); Vec(serlaplace(A))
