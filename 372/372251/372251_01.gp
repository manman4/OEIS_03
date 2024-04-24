\\ a(n) = 1/2 * Sum_{k=0..n} (n+k/2+1/2)^(n-1) * binomial(n,k).
a(n) =1/2 * sum(k=0, n, (n+k/2+1/2)^(n-1) * binomial(n,k));
for(n=0, 17, print1(a(n), ", "))

