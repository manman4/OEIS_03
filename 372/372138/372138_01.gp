\\ a(n) = Sum_{k=0..n} 4^(n-k) * binomial(n+2*k,k) * binomial(k/2,n-k)/(n+k+1).
a(n) = sum(k=0, n, 4^(n-k) * binomial(n+2*k,k) * binomial(k/2,n-k)/(n+k+1));
for(n=0, 22, print1(a(n), ", "))

