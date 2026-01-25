M=30;

\\ E.g.f. A(x) satisfies A(x) = 1 - x*A(x) * log(1-x*A(x))^3.
seq(n) = my(A=1); for(i=1, n, A=1 - x*A*log(1 - x*A + x*O(x^n))^3); Vec(serlaplace(A));
seq(M) 