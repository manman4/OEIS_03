\\ a(n) = Sum_{k=0..floor(n/4)} binomial(k,n-4*k)^2.
a(n) = sum(k=0, n\4, binomial(k, n-4*k)^2);
for(n=0, 53, print1(a(n),", "))



