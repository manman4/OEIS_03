\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(n+1,k) * binomial(7*n-3*k+7,n-k).
a(n) = sum(k=0, n, binomial(n+1, k)*binomial(7*n-3*k+7, n-k))/(n+1);
for(n=0, 18, print1(a(n), ", ")); 