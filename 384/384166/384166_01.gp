M=16;

\\ a(n) = Product_{k=0..n-1} (3*n+4*k).
a(n) = prod(k=0, n-1, 3*n+4*k);
for(n=0, M, print1(a(n),", "));
