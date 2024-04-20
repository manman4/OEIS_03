\\ a(n) = Sum_{k=0..n} 4^(n-k) * binomial(2*k,k) * binomial(k/2,n-k)/(k+1).
a(n) = sum(k=0, n, 4^(n-k) * binomial(2*k,k) * binomial(k/2,n-k)/(k+1));
for(n=0, 25, print1(a(n), ", "))

