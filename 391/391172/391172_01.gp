\\ a(n) = Sum_{k=0..n} (k+2) * binomial(k+2,2) * binomial(3*n-2*k+2,n-k)/(3*n-2*k+2).
a(n) = sum(k=0, n, (k+2)*binomial(k+2,2)*binomial(3*n-2*k+2, n-k)/(3*n-2*k+2));
for(n=0, 23, print1(a(n), ", "));

