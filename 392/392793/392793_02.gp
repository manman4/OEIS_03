M=30;

\\ E.g.f. A(x) satisfies A(x) = 1/(1 - log(1-(x*A(x))^4)^2/(x*A(x))^3).
seq(n) = my(A=1); for(i=1, n, A=1/(1 - log(1-(x*A)^4)^2/(x*A)^3) + x*O(x^n)); Vec(serlaplace(A));
seq(M) 