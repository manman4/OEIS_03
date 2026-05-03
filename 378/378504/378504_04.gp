\\ a(n) = ((n+1)/6) * (binomial(3*n+2,n) + Sum_{k=0..n+1} 2^(n+1-k) * binomial(3*n+3,k)).
a(n) = ((n+1)/6) * (binomial(3*n+2,n) + sum(k=0, n+1, 2^(n+1-k) * binomial(3*n+3,k)));
for(n=0, 20, print1(a(n), ", "));