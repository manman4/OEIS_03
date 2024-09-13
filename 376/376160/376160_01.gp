\\ a(n) = Sum_{k=0..n} binomial(n+11*k+2,n-k) * binomial(4*k,k)/(3*k+1).
a(n) = sum(k=0, n, binomial(n+11*k+2,n-k) * binomial(4*k,k)/(3*k+1));
for(n=0, 19, print1(a(n), ", "))

