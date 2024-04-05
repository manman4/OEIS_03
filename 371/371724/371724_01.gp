\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(n/2+3*k/2+3/2,n)/(n/3+k+1).
a(n) = sum(k=0, n, binomial(n, k)*binomial(n/2+3*k/2+3/2, n)/(n/3+k+1));
for(n=0, 25, print1(a(n), ", "))



