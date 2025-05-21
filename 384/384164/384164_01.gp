M=17;

\\ a(n) = Product_{k=0..n-1} (3*n+k).
a(n) = prod(k=0, n-1, 3*n+k);
for(n=0, M, print1(a(n),", "));
