M=30;

\\ E.g.f. A(x) satisfies A(x) = 1 - (A(x)/x) * log(1 - x^2*A(x)).
seq(n) = my(A=1); for(i=1, n, A=1 - (A/x)*log(1 - x^2*A) + x*O(x^n)); Vec(serlaplace(A));
seq(M) 