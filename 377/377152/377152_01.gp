\\ a(n) = Sum_{k=0..n} binomial(k+4,4) * binomial(k,n-k)^2.
a(n) = sum(k=0, n, binomial(k+4, 4) * binomial(k, n-k)^2);
for(n=0, 25, print1(a(n), ", "))
