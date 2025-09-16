\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(n+k+4,n).
a(n) = sum(k=0, n, binomial(n,k) * binomial(n+k+4,n));
for(n=0, 22, print1(a(n),", "));


