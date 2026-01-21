M=30;

\\ E.g.f. A(x) satisfies A(x) = x + (exp(A(x)) - 1)^4.
seq(n) = my(A=x); for(i=1, n, A=x + (exp(A) - 1)^4 + x*O(x^n)); Vec(serlaplace(A));
seq(M) 