M=30;

\\ E.g.f. A(x) satisfies A(x) = 1 + x*A(x) * (exp(x^2*A(x)^2) - 1).
seq(n) = my(A=1); for(i=1, n, A=1 + x*A * (exp(x^2*A^2) - 1) + x*O(x^n)); Vec(serlaplace(A));
seq(M) 