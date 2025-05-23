M=11;

\\ A(n,k) = Product_{j=0..n-1} (n-k*j).
a(n, k) = prod(j=0, n-1, n-k*j);
for(n=0, M, for(k=0, n-1, print1(a(k,n-k),", ")));

\\ A(k*n,k) = 0 for n > 0 and k > 1.
for(k=2, 8, for(n=1, 10, print1(a(k*n,k),", "));print);
 

