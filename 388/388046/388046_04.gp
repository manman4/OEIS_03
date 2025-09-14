\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(4*n+k+1,n).
a(n) = sum(k=0, n, binomial(n,k) * binomial(4*n+k+1,n));
for(n=0, 17, print1(a(n),", "));


