\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} binomial(n+k,k) * binomial(n+k-1,n-3*k).
a(n) = sum(k=0, n\3, binomial(n+k, k)*binomial(n+k-1,n-3*k))/(n+1);
for(n=0, 30, print1(a(n), ", "));