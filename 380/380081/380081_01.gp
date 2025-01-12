M=21;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..n} 3^k * k^(n-k) * binomial(n/3+1/3,k)/(n-k)!.
a(n) = n! * sum(k=0, n, 3^k * k^(n-k) * binomial(n/3+1/3,k)/(n-k)!)/(n+1);
for(n=0, M, print1(a(n),", "));



