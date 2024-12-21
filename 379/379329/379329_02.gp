M=25;

\\ G.f. A(x) satisfies A(x) = 1/sqrt( (1 - 2*x) * (1 - 2*x*A(x)) ).
seq(n) = my(A=1); for(i=1, n, A=1/sqrt( (1 - 2*x) * (1 - 2*x*A) ) + x*O(x^n)); Vec(A);
seq(M)

