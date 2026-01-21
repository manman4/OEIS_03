M=30;

\\ E.g.f. A(x) satisfies A(x) = x - log(1 - A(x))^3.
seq(n) = my(A=x); for(i=1, n, A=x - log(1 - A)^3 + x*O(x^n)); Vec(serlaplace(A));
seq(M) 