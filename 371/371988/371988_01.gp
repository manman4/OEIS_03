\\ a(n) = 16^n * Sum_{k=0..n} binomial(n,k) * binomial(k/4+1/4,n)/(k+1).
a(n) = 16^n * sum(k=0, n, binomial(n, k)*binomial(k/4+1/4, n)/(k+1));
for(n=0, 18, print1(a(n), ", "))
