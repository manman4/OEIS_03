M=30;
\\ E.g.f. A(x) satisfies A(x) = 1 - x * A(x)^2 * log(1 - (x * A(x))^4).
my(A=1, N=M); for(k=1, N, A=1 - x * A^2 * log(1 - (x * A)^4)+x*O(x^N)); Vec(serlaplace(A))
