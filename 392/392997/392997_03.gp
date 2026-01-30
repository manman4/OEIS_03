M=30;

\\ E.g.f. A(x) satisfies A(x) = 1/(exp(-x*A(x)) - (x*A(x))^2).
seq(n) = my(A=1); for(i=1, n, A=1/(exp(-x*A) - (x*A)^2) + x*O(x^n)); Vec(serlaplace(A));
seq(M) 
