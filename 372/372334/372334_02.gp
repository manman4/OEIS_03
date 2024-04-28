\\ a(n) = Sum_{k=1..n} (3*k)^(k-1) * binomial(n,k).
a(n) = sum(k=1, n, (3*k)^(k-1) * binomial(n,k));
for(n=0, 17, print1(a(n), ", "))

