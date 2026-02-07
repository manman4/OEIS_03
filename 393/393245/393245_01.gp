\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*k,k) * binomial(n,4*k).
a(n) = sum(k=0, n\4, binomial(2*k, k)*binomial(n, 4*k));
for(n=0, 33, print1(a(n),", "));

