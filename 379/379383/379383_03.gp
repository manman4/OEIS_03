\\ a(n) = 2^n * Sum_{k=0..n} binomial(n,k) * binomial(n/2+2*k+1/2,n)/(n+4*k+1).
a(n) = 2^n * sum(k=0, n, binomial(n,k) * binomial(n/2+2*k+1/2,n)/(n+4*k+1));
for(n=0, 25, print1(a(n),", "))
