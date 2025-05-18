M=9;

\\ A(n,k) = [x^n] Product_{j=0..n} (1 + (k*n+j)*x).
a(n, k) = polcoef(prod(j=0, n, (1 + (k*n+j)*x) + x*O(x^(n))), n);
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));


