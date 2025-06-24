M=21;

a185951(n, k) = binomial(n, k)/2^k*sum(j=0, k, (2*j-k)^(n-k)*binomial(k, j));

\\ a(n) = 1/2 * Sum_{k=0..n} (k+2)! * i^(n-k) * A185951(n,k), where i is the imaginary unit.
a(n) = sum(k=0, n, (k+2)!*I^(n-k)*a185951(n, k))/2;
for(n=0, M, print1(a(n), ", "))

