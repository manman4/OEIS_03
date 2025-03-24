{a(n)=sum(k=0, n, (binomial(2*n+1, k)-binomial(2*n+1, k-1))^3)/binomial(2*n+1, n)}
\\ a(n) = binomial(2*n,n)/2 * A183069(n) for n > 0.
a(n) = binomial(2*n,n)/2 * b(n);
for(n=0, 17, print1(a(n), ", "))

\\ a(n) = binomial(2*n,n) * (4 * binomial(2*n,n)^2 - 3 * A112029(n)).
a(n) = binomial(2*n,n) * (4 * binomial(2*n,n)^2 - 3 * b(n));
for(n=0, 17, print1(a(n), ", "))

