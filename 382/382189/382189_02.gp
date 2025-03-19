M=26;

\\ G.f. A(x) satisfies A(x) = 1/(1/A(x^3)^2 - 4*x)^(1/2).
a(n) = my(A=1); for(i=1, n, A = 1/(1/subst(A, x, x^3)^2 - 4*x + x*O(x^n))^(1/2) ); Vec(A);
a(M)




