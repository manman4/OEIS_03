M=30;

\\ G.f. A(x) satisfies A(x) = (1 + 4*x*A(x)^7)^(1/2).
seq(n) = my(A=1); for(i=1, n, A=(1+4*x*A^7 + x*O(x^n))^(1/2)); Vec(A);
seq(M) 

\\ G.f. A(x) satisfies A(x) = 1/A(-x*A(x)^12).
seq(n) = my(A=1); for(i=1, n, A=(1+4*x*A^7 + x*O(x^n))^(1/2)); Vec(A - 1/subst(A, x, -x*A^12));
seq(M) 
