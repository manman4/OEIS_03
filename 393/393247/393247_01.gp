\\ a(n) = Sum_{k=0..floor(n/5)} binomial(2*k,k) * binomial(n-2*k,3*k).
a(n) = sum(k=0, n\5, binomial(2*k, k)*binomial(n-2*k, 3*k));
for(n=0, 36, print1(a(n),", "));

