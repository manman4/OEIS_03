\\ a(n) = binomial(n+2,2) * Sum_{k=0..n} binomial(n,k) * binomial(k,n-k).
\\ a(n) = binomial(n+2,2) * A002426(n).
a(n) = binomial(n+2,2) * sum(k=0, n, binomial(n,k)*binomial(k,n-k));
for(n=0, 26, print1(a(n),", "))
for(n=0, 28, print1(a(n)/binomial(n+2,2),", ")) 
