M=17;

\\ a(n) = Product_{k=0..n-1} (2*n+3*k).
a(n) = prod(k=0, n-1, 2*n+3*k);
for(n=0, M, print1(a(n),", "));
