\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(n+k,k) * binomial(2*n+3*k,n-k).
a(n) = sum(k=0, n, binomial(n+k, k)*binomial(2*n+3*k,n-k))/(n+1);
for(n=0, 21, print1(a(n), ", "));