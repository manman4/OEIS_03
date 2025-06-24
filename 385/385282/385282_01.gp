M=18;

a185951(n, k) = binomial(n, k)/2^k*sum(j=0, k, (2*j-k)^(n-k)*binomial(k, j));
a007559(n) = prod(k=0, n-1, 3*k+1);
\\ a(n) = Sum_{k=0..n} A007559(k) * 3^(n-k) * A185951(n,k).
a(n) = sum(k=0, n, a007559(k) * 3^(n-k) * a185951(n, k));
for(n=0, M, print1(a(n), ", "))

