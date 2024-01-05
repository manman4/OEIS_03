\\ a(n) = n! * (1 + Sum_{k=0..n} (-1)^k * binomial(k+2,3) / k!).
a(n) = n! * (1 + sum(k=0, n, (-1)^k * binomial(k+2, 3) / k!));
for(n=0, 22, print1(a(n), ", "));
