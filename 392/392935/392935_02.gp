M=30;

\\ E.g.f. A(x) satisfies A(x) = 1 - x^3*A(x)^5 * log(1 - x^2*A(x)^2).
seq(n) = my(A=1); for(i=1, n, A=1 - x^3*A^5*log(1 - x^2*A^2) + x*O(x^n)); Vec(serlaplace(A));
seq(M) 