M=22;

\\ a(n) = Product_{k=0..n-1} (3*n-4*k).
a(n) = prod(k=0, n-1, 3*n-4*k);
for(n=0, M, print1(a(n),", "));

\\ a(n) = 3 * (-1)^(n-1) * A383996(n) for n > 0.
a383996(n) = prod(k=0, n-1, n-4*k);
for(n=0, M, print1(a(n) - 3*(-1)^(n-1)*a383996(n),", "));

\\ a(4*n) = 0 for n > 0.
for(n=0, 100, print1(a(4*n),", "));
