M=19;

a185951(n, k) = binomial(n, k)/2^k*sum(j=0, k, (2*j-k)^(n-k)*binomial(k, j));
\\ a(n) = 2 * Sum_{k=0..n} (2*n-2*k+2)^(k-1) * A185951(n,k).
a(n) = 2 * sum(k=0, n, (2*n-2*k+2)^(k-1)*a185951(n, k));
for(n=0, M, print1(a(n),", "));

