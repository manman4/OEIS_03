\\ a(n) = Sum_{k=0..n} 4^k * binomial(k/2+1/2,k) * binomial(k,n-k)/(k+1).
a(n) = sum(k=0, n, 4^k * binomial(k/2+1/2,k) * binomial(k,n-k)/(k+1));
for(n=0, 34, print1(a(n), ", "))


