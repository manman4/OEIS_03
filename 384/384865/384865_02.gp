M=10;

\\ G.f. satisfies: A(x) = 1 + x/A(-x*A(x)^3).
a(n, k) = my(A=1); for(i=1, n, A = 1 + x / subst(A, x, -x*A^3) + x*O(x^n) ); polcoef(A^k, n);
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));

