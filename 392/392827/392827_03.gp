M=30;

\\ E.g.f. A(x) satisfies A(x) = 1 + (x*A(x))^2 * (exp(x*A(x)) - 1)^2.
seq(n) = my(A=x); for(i=1, n, A=1 + (x*A)^2 * (exp(x*A) - 1)^2 + x*O(x^n)); Vec(serlaplace(A));
seq(M) 