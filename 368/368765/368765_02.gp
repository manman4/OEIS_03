\\ a(n) = n! * (1 + Sum_{k=0..n} (-1)^k * k / k!).
a(n) = n! * (1 + sum(k=0, n, (-1)^k * k / k!));
for(n=0, 23, print1(a(n), ", "));
