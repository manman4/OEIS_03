M=30;

\\ E.g.f. A(x) satisfies A(x) = 1 + log(1-(x*A(x))^2)^2/x.
seq(n) = my(A=1); for(i=1, n, A=1 + log(1-(x*A)^2)^2/x + x*O(x^n)); Vec(serlaplace(A));
seq(M) 