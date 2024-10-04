\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*n-7*k,k) * binomial(2*n-8*k,n-4*k).
a(n) = sum(k=0, n\4, binomial(2*n-7*k, k) * binomial(2*n-8*k, n-4*k));
for(n=0, 26, print1(a(n),", "))



