M=20;

\\ a(n) = Product_{k=0..n-1} (n-5*k).
a(n) = prod(k=0, n-1, n-5*k);
for(n=0, M, print1(a(n),", "));

\\ a(5*n) = 0 for n > 0.
for(n=1, 100, print1(a(5*n),", "));
