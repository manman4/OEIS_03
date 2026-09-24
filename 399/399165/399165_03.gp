M=30;

\\ G.f. A(x) satisfies A(x) = 1/(1 - 4*x*A(x))^(5/2).
seq(n) = my(A=1); for(i=1, n, A=1/(1 - 4*x*A)^(5/2) + x*O(x^n)); Vec(A);
seq(M)

\\ G.f. A(x) satisfies A(x) = (1 + 4*x*A(x)^(7/5))^(5/2).
seq(n) = my(A=1); for(i=1, n, A=(1+4*x*A^(7/5))^(5/2) + x*O(x^n)); Vec(A);
seq(M) 

\\ G.f.: B(x)^5 where B(x) is the g.f. of A399168.
seq(n) = my(A=1); for(i=1, n, A=(1+4*x*A^(7/5))^(5/2) + x*O(x^n)); Vec(A^(1/5));
seq(M) 

\\ G.f. A(x) satisfies A(x) = 1/A(-x*A(x)^(12/5)).
seq(n) = my(A=1); for(i=1, n, A=(1+4*x*A^(7/5))^(5/2) + x*O(x^n)); Vec(A - 1/subst(A, x, -x*A^(12/5)));
seq(M) 
