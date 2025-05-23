M=17;

\\ a(n) = Product_{k=0..n-1} (n-4*k).
a(n) = prod(k=0, n-1, n-4*k);
for(n=0, M, print1(a(n),", "));

\\ a(4*n) = 0 for n > 0.
for(n=1, 100, print1(a(4*n),", "));
