M=33;

\\ G.f. A(x) satisfies A(x) = 1/( 1/A(x^3) - x/(1-x) ).
a(n) = my(A=1); for(i=1, n, A = 1/( 1/subst(A, x, x^3) - x/(1-x) + x*O(x^n) ) ); Vec(A);
a(M)