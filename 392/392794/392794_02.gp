M=30;

\\ E.g.f. A(x) satisfies A(x) = 1 + (exp((x*A(x))^2) - 1)^2/x.
seq(n) = my(A=x); for(i=1, n, A=1 + (exp((x*A)^2) - 1)^2/x + x*O(x^n)); Vec(serlaplace(A));
seq(M) 