\\ a(n) = 16^n * Sum_{k=0..n} binomial(n,k) * binomial(n/4+k/4+1/4,n)/(n+k+1).
a(n) = 16^n * sum(k=0, n, binomial(n, k)*binomial(n/4+k/4+1/4, n)/(n+k+1));
for(n=0, 19, print1(a(n), ", "))
