\\ a(n) = Sum_{k=1..n} (2*k)^(n-k) * (k-1)! * binomial(n,k).
a(n) = sum(k=1, n, (2*k)^(n-k) * (k-1)! * binomial(n,k));
for(n=0, 18, print1(a(n), ", "));

