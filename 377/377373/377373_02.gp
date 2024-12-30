M=20;

\\ a(n) = n! * Sum_{k=0..n} (-1)^k * 2^(n-k) * (k+1)^(k-1) * binomial(n,k)/k!.
a(n) = n! * sum(k=0, n, (-1)^k * 2^(n-k) * (k+1)^(k-1) * binomial(n,k)/k!);
for(n=0, M, print1(a(n),", "))                 

a376106(n) = n!*sum(k=1, n, 2^(n-k)*(-k)^(k-1)*binomial(n-1, k-1)/k!);
\\ a(n) = A376106(n+1)/(n+1).
b(n) = a376106(n+1)/(n+1);
for(n=0, 100, print1(a(n)-b(n), ", "))