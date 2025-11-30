\\ a(n) = (1/(3*n+2)) * Sum_{k=0..n} (3*k+2) * binomial(3*n+2,n-k).
a(n) = (1/(3*n+2))*sum(k=0, n, (3*k+2)*binomial(3*n+2, n-k));
for(n=0, 23, print1(a(n), ", "));



