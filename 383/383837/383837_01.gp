\\ a(n) = (1/(2^n*n!)) * Sum_{k=0..n} (-1)^k * (n-2*k)^(3*n) * binomial(n,k).
a(n) = (1/(2^n*n!)) * sum(k=0, n, (-1)^k * (n-2*k)^(3*n) * binomial(n,k));
for(n=0, 14, print1(a(n),", "));

