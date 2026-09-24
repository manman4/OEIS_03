M=30;

\\ G.f. A(x) satisfies A(x) = 1 + 4*x*A(x)^(7/2).
seq(n) = my(A=1); for(i=1, n, A=1+4*x*A^(7/2) + x*O(x^n)); Vec(A);
seq(M) 

\\ G.f.: B(x)^2 where B(x) is the g.f. of A399168.
seq(n) = my(A=1); for(i=1, n, A=1+4*x*A^(7/2) + x*O(x^n)); Vec(A^(1/2));
seq(M) 

\\ G.f. A(x) satisfies A(x) = 1/A(-x*A(x)^6).
seq(n) = my(A=1); for(i=1, n, A=1+4*x*A^(7/2) + x*O(x^n)); Vec(A - 1/subst(A, x, -x*A^6));
seq(M) 
