\\ a(n) = Sum_{k=0..n} (-3)^k * 13^(n-k) * (2*k+1) * binomial(2*k,k) * binomial(n+1,n-k).
a(n) = sum(k=0, n, (-3)^k * 13^(n-k) * (2*k+1) * binomial(2*k,k) * binomial(n+1,n-k));
for(n=0, 20, print1(a(n), ", "));


