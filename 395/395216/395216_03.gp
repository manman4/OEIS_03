\\ a(n) = (2*n)! * Sum_{k=0..n} 16^k * (-3)^(n-k) * binomial(n+k-1/2,n+k) * binomial(n+k,2*k).
a(n) = (2*n)! * sum(k=0, n, 16^k * (-3)^(n-k) * binomial(n+k-1/2, n+k) * binomial(n+k, 2*k));
for(n=0, 15, print1(a(n),", "));