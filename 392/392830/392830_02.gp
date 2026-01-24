M=30;

\\ E.g.f. A(x) satisfies A(x) = 1/(1 - x*A(x)*log(1-x)^2).
seq(n) = my(A=x); for(i=1, n, A=1/(1 - x*A*log(1-x  + x*O(x^n))^2)); Vec(serlaplace(A));
seq(M) 