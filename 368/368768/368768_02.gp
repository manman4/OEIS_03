\\ a(n) = n! * (1 + Sum_{k=0..n} (-1)^k * binomial(k+3,4) / k!).
a(n) = n! * (1 + sum(k=0, n, (-1)^k * binomial(k+3, 4) / k!));
for(n=0, 22, print1(a(n), ", "));
