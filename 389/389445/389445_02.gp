\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/5)} binomial(n+k,k) * binomial(n+k+1,n-5*k).
a(n) = sum(k=0, n\5, binomial(n+k, k)*binomial(n+k+1, n-5*k))/(n+1);
for(n=0, 31, print1(a(n), ", "));