\\ a(n) = Sum_{k=0..n} binomial(k+3,3) * binomial(k,n-k)^2.
a(n) = sum(k=0, n, binomial(k+3, 3) * binomial(k, n-k)^2);
for(n=0, 26, print1(a(n), ", "))
