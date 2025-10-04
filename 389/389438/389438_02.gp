\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(n+k,k) * binomial(n+3*k+1,n-k).
a(n) = sum(k=0, n, binomial(n+k, k)*binomial(n+3*k+1, n-k))/(n+1);
for(n=0, 22, print1(a(n), ", "));