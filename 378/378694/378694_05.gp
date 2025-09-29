\\ a(n) = ( (-1)^n / (6*n+1) ) * Sum_{k=0..n} (-2)^(n-k) * binomial(6*n+1,k) * binomial(7*n-k,n-k).
a(n) = (-1)^n*sum(k=0, n, (-2)^(n-k)*binomial(6*n+1, k)*binomial(7*n-k, n-k))/(6*n+1);
for(n=0, 20, print1(a(n),", "));    