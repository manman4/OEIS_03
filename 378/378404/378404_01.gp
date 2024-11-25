\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(n+3*k,n-k).
a(n) = sum(k=0, n, binomial(n,k) * binomial(n+3*k,n-k));
for(n=0, 22, print1(a(n), ", "))

