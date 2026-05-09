\\ T(n,k) = [x^k] Product_{j=0..n} (1 + j^4*x).
a395066(n, k) = polcoef(prod(j=0, n, 1+j^4*x), k);
\\ a(n) = A395066(n,n-2)
b(n) = a395066(n, n-2);
for(n=0, 30, print1(b(n),", "));

\\ [x^(n-2)] Product_{k=0..n} (1 + k^4*x).
a(n) = polcoef(prod(k=0, n, 1+k^4*x), n-2);
for(n=0, 50, print1(a(n)-b(n),", "));