M=21;

\\ G.f. A(x) satisfies A(x) = (A(x^3)^3 + 9*x)^(1/3).
a(n) = my(A=1); for(i=1, n, A = (subst(A, x, x^3)^3 + 9*x + x*O(x^n))^(1/3) ); Vec(A);
a(M)





