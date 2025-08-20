\\ a(n) = Sum_{k=0..n} (-1)^k * 7^(n-k) * (2*k+1) * binomial(2*k,k) * binomial(n+2,n-k).
a(n) = sum(k=0, n, (-1)^k * 7^(n-k) * (2*k+1) * binomial(2*k,k) * binomial(n+2,n-k));
for(n=0, 25, print1(a(n),", "));
