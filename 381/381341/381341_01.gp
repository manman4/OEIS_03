M=23;

a185951(n, k) = binomial(n, k)/2^k*sum(j=0, k, (2*j-k)^(n-k)*binomial(k, j));
for(n=0, 10, for(k=0, n, print1(a185951(n, k),", ")); print);

\\ a(n) = Sum_{k=0..n} 2^((n-k)/2) * A185951(n,k).
a(n) = sum(k=0, n, 2^((n-k)/2) * a185951(n, k));
for(n=0, M, print1(a(n), ", "));



