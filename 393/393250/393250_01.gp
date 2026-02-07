\\ a(n) = Sum_{k=0..floor(n/6)} binomial(2*k,k) * binomial(n-3*k,3*k).
a(n) = sum(k=0, n\6, binomial(2*k, k)*binomial(n-3*k, 3*k));
for(n=0, 38, print1(a(n),", "));

