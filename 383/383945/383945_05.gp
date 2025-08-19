\\ a(n) = Sum_{k=0..n} (-2)^k * 7^(n-k) * binomial(2*k,k) * binomial(n+2,n-k).
a(n) = sum(k=0, n, (-2)^k * 7^(n-k) * binomial(2*k,k) * binomial(n+2,n-k));
for(n=0, 25, print1(a(n), ", "));


