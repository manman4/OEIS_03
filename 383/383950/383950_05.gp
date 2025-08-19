\\ a(n) = Sum_{k=0..n} 2^(n-k) * binomial(2*k,k) * binomial(n+1,n-k).
a(n) = sum(k=0, n, 2^(n-k) * binomial(2*k,k) * binomial(n+1,n-k));
for(n=0, 25, print1(a(n), ", "));


