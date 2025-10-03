\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/5)} binomial(n+k,k) * binomial(2*n-3*k,n-5*k).
a(n) = sum(k=0, n\5, binomial(n+k, k)*binomial(2*n-3*k,n-5*k))/(n+1);
for(n=0, 27, print1(a(n), ", "));