M=19;

a185951(n, k) = binomial(n, k)/2^k*sum(j=0, k, (2*j-k)^(n-k)*binomial(k, j));
\\ a(n) = Sum_{k=0..n} 2^k * k! * A185951(n,k).
a(n) = sum(k=0, n, 2^k * k! * a185951(n, k));
for(n=0, M, print1(a(n), ", "))

