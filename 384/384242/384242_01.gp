M=20;

\\ a(n) = Product_{k=0..n-1} (4*n-5*k).
a(n) = prod(k=0, n-1, 4*n-5*k);
for(n=0, M, print1(a(n),", "));

\\ a(n) = 4 * (-1)^(n-1) * A383997(n) for n > 0.
a383997(n) = prod(k=0, n-1, n-5*k);
for(n=0, M, print1(a(n) - 4*(-1)^(n-1)*a383997(n),", "));

\\ a(5*n) = 0 for n > 0.
for(n=0, 100, print1(a(5*n),", "));
