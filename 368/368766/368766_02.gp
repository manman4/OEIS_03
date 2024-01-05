\\ a(n) = n! * (1 + Sum_{k=0..n} (-1)^k * binomial(k+1,2) / k!).
a(n) = n! * (1 + sum(k=0, n, (-1)^k * binomial(k+1, 2) / k!));
for(n=0, 22, print1(a(n), ", "));
