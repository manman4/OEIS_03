M=17;

\\ a(n) = Product_{k=0..n-1} (3*n+k-2).
a(n) = prod(k=0, n-1, 3*n+k-2);
for(n=0, M, print1(a(n),", "));
