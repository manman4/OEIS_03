\\ a(n) = Sum_{k=0..n} (2*k-1)^(k-1) * binomial(n,k).
a(n) = sum(k=0, n, (2*k-1)^(k-1) * binomial(n,k));
for(n=0, 17, print1(a(n), ", "))

