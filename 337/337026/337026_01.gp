\\ a(n) = 1 + Sum_{k=1..n} 2^(k-1) * binomial(n,k) * a(n-k). 
a(n) = 1 + sum(k=1, n, 2^(k-1)*binomial(n, k)*a(n-k));
for(n=0, 18, print1(a(n), ", ")); 
