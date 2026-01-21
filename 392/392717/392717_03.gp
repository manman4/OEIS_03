M=30;

\\ E.g.f. A(x) satisfies A(x) = log(1 + x + A(x)^4).
seq(n) = my(A=x); for(i=1, n, A=log(1 + x + A^4) + x*O(x^n)); Vec(serlaplace(A));
seq(M) 