M=30;

\\ E.g.f.: exp(B(x)) - 1, where B(x) is the e.g.f. of A392724.
seq(n) = my(A=x); for(i=1, n, A=x + (exp(A) - 1)^4 + x*O(x^n)); Vec(serlaplace(exp(A) - 1));
seq(M) 