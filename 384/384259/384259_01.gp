M=17;

\\ a(n) = Product_{k=0..n-1} (n+4*k+3).
a(n) = prod(k=0, n-1, n+4*k+3);
for(n=0, M, print1(a(n),", "));


