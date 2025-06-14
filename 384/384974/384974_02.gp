M=24;

\\ G.f. A(x) satisfies A(x) = 1 + x*A(x)/A(-x*A(x)^3)^2.
a(n, k) = my(A=1); for(i=1, n, A = 1 + x*A / subst(A, x, -x*A^3)^2 + x*O(x^n) ); polcoef(A^k, n);
for(n=0, M, print1(a(n, 1),", "));

