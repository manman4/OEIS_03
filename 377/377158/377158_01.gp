\\ a(n) = Sum_{k=0..n} binomial(k+6,6) * binomial(k,n-k)^2.
a(n) = sum(k=0, n, binomial(k+6, 6) * binomial(k, n-k)^2);
for(n=0, 24, print1(a(n), ", "))
