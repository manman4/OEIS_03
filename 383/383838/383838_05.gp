\\ a(n) = (1/8!) * Sum_{k=0..8} (-1)^k * (4-k)^(2*n+8) * binomial(8,k).
a(n) = (1/8!) * sum(k=0, 8, (-1)^k * (4-k)^(2*n+8) * binomial(8,k));
for(n=0, 20, print1(a(n),", "));

