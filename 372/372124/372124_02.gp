\\ a(n) = Sum_{k=0..n} 9^(n-k) * binomial(2*k,k) * binomial(k/3,n-k)/(k+1).
a(n) = sum(k=0, n, 9^(n-k) * binomial(2*k,k) * binomial(k/3,n-k)/(k+1));
for(n=0, 22, print1(a(n), ", "))

