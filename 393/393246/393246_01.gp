\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*k,k) * binomial(n-k,3*k).
a(n) = sum(k=0, n\4, binomial(2*k, k)*binomial(n-k, 3*k));
for(n=0, 34, print1(a(n),", "));

