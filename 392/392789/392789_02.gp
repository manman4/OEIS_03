M=30;

\\ E.g.f. A(x) satisfies A(x) = 1/(1 + (1 - exp((x*A(x))^3))/(x*A(x))^2).
seq(n) = my(A=x); for(i=1, n, A=1/(1 + (1 - exp((x*A)^3))/(x*A)^2) + x*O(x^n)); Vec(serlaplace(A));
seq(M) 