\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(n+3*k,n).
a(n) = sum(k=0, n, binomial(n,k) * binomial(n+3*k,n));
for(n=0, 20, print1(a(n), ", "))

