\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(n/2+5*k/2+5/2,n)/(n/5+k+1).
a(n) = sum(k=0, n, binomial(n, k)*binomial(n/2+5*k/2+5/2, n)/(n/5+k+1));
for(n=0, 21, print1(a(n), ", "))



