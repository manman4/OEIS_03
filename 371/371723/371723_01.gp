\\ a(n) = 2 * Sum_{k=0..n} binomial(n,k) * binomial(n/2+5*k/2+2,n)/(n/2+5*k/2+2).
a(n) = 2 * sum(k=0, n, binomial(n, k)*binomial(n/2+5*k/2+2, n)/(n/2+5*k/2+2));
for(n=0, 21, print1(a(n), ", "))



