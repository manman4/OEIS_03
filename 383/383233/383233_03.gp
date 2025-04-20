M=19;

\\ a(n) = 5^(n-1) * n! * Sum_{k=0..n-1} (-1)^k * binomial(-3/5,k)/(n-k).
a(n) = 5^(n-1) * n! * sum(k=0, n-1, (-1)^k * binomial(-3/5,k)/(n-k));
for(n=0, M, print1(a(n), ", "));



