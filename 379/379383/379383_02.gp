M=30;

\\ G.f. A(x) satisfies A(x) = sqrt( (1 + 2*x*A(x))/(1 - 2*x*A(x)^3) ).
seq(n) = my(A=1); for(i=1, n, A=sqrt( (1 + 2*x*A) / (1 - 2*x*A^3) ) + x*O(x^n)); Vec(A);
seq(M)
