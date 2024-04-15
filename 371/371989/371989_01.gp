\\ a(n) = 9^n * Sum_{k=0..n} binomial(n,k) * binomial(n/3+k/3+1/3,n)/(n+k+1).
a(n) = 9^n * sum(k=0, n, binomial(n, k)*binomial(n/3+k/3+1/3, n)/(n+k+1));
for(n=0, 22, print1(a(n), ", "))
