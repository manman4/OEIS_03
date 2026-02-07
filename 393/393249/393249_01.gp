\\ a(n) = Sum_{k=0..floor(n/5)} binomial(2*k,k) * binomial(n-4*k,k).
a(n) = sum(k=0, n\5, binomial(2*k, k)*binomial(n-4*k, k));
for(n=0, 43, print1(a(n),", "));

