M=30;

\\ G.f. A(x) satisfies A(x) = ( A(x^2)^2 + 4*x/(1-x)^2 )^(1/2).
a(n) = my(A=1); for(i=1, n, A = ( subst(A, x, x^2)^2 + 4*x/(1-x)^2 + x*O(x^n) )^(1/2) ); Vec(A);
a(M)