M=30;

\\ G.f. A(x) satisfies A(x) = 1/(1 - 4*x*A(x))^(3/2).
seq(n) = my(A=1); for(i=1, n, A=1/(1 - 4*x*A)^(3/2) + x*O(x^n)); Vec(A);
seq(M)

\\ G.f. A(x) satisfies A(x) = (1 + 4*x*A(x)^(5/3))^(3/2).
seq(n) = my(A=1); for(i=1, n, A=(1+4*x*A^(5/3))^(3/2) + x*O(x^n)); Vec(A);
seq(M) 

\\ G.f.: B(x)^3 where B(x) is the g.f. of A245112.
seq(n) = my(A=1); for(i=1, n, A=(1+4*x*A^(5/3))^(3/2) + x*O(x^n)); Vec(A^(1/3));
seq(M) 

\\ G.f. A(x) satisfies A(x) = 1/A(-x*A(x)^(8/3)).
seq(n) = my(A=1); for(i=1, n, A=(1+4*x*A^(5/3))^(3/2) + x*O(x^n)); Vec(A - 1/subst(A, x, -x*A^(8/3)));
seq(M) 
