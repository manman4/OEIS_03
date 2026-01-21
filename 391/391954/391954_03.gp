M=30;

\\ E.g.f. A(x) satisfies A(x) = exp(x + A(x)^3) - 1.
seq(n) = my(A=x); for(i=1, n, A=exp(x + A^3) - 1 + x*O(x^n)); Vec(serlaplace(A));
seq(M) 