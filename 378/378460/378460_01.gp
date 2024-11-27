\\ a(n) = Sum_{k=0..n} binomial(n+k-1,k) * binomial(2*n+k-1,n-k).
a(n) = sum(k=0, n, binomial(n+k-1,k) * binomial(2*n+k-1,n-k));
for(n=0, 21, print1(a(n), ", "))

