\\ a(n) = (1/n!) * Sum_{k=0..n} (-1)^(n-k) * (n+k)^(2*n) * binomial(n,k).
a(n) = sum(k=0, n, (-1)^(n-k) * (n+k)^(2*n) * binomial(n, k)) / n!;
for(n=0, 16, print1(a(n),", "));


