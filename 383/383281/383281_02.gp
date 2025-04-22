\\ a(n) = (n!)^2 * Sum_{k=0..n} (-1)^k * (1/2)^(n-k) * binomial(-3/2,k)/(n-k)!.
a(n) = (n!)^2 * sum(k=0, n, (-1)^k * (1/2)^(n-k) * binomial(-3/2,k)/(n-k)!);
for(n=0, 20, print1(a(n),", "))