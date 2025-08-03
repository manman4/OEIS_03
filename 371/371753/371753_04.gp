\\ a(n) = Sum_{k=0..n} (-1)^(n-k) * binomial(4*n+k,k). 
a(n) = sum(k=0, n, (-1)^(n-k) * binomial(4*n+k,k));
for(n=0, 20, print1(a(n),", "))