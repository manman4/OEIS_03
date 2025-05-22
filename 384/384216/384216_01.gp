M=11;

\\ A(n,k) = Product_{j=0..n-1} (n-k*j).
a(n, k) = prod(j=0, n-1, n-k*j);
for(n=0, M, for(k=0, n-1, print1(a(k,n-k),", ")));
