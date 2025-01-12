M=20;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..n} 2^k * k^(n-k) * binomial(n/2+1/2,k)/(n-k)!.
a(n) = n! * sum(k=0, n, 2^k * k^(n-k) * binomial(n/2+1/2,k)/(n-k)!)/(n+1);
for(n=0, M, print1(a(n),", "));



