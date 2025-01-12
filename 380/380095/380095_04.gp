M=17;

\\ a(n) = (n!/(2*n+1)) * Sum_{k=0..n} (-2)^k * k^(n-k) * binomial(-n-1/2,k)/(n-k)!.
a(n) = (n!/(2*n+1)) * sum(k=0, n, (-2)^k * k^(n-k) * binomial(-n-1/2,k)/(n-k)!);
for(n=0, M, print1(a(n),", "));



