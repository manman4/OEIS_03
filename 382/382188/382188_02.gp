M=21;

\\ G.f. A(x) satisfies A(x) = 1/(1/A(x^2)^3 - 9*x)^(1/3).
a(n) = my(A=1); for(i=1, n, A = 1/(1/subst(A, x, x^2)^3 - 9*x + x*O(x^n))^(1/3) ); Vec(A);
a(M)


