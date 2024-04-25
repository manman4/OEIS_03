\\ a(n) = Sum_{k=0..n} (5*k/2)^(n-k) * (5*k/2+1)^(k-1) * binomial(n,k).
a(n) = sum(k=0, n, (5*k/2)^(n-k) * (5*k/2+1)^(k-1) * binomial(n,k));
for(n=0, 16, print1(a(n), ", "))

