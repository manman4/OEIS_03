\\ a(n) = binomial(n+3,3) * Sum_{k=0, floor(n/2)} binomial(n,n-2*k) * binomial(2*k,k).
a(n) = binomial(n+3,3) * sum(k=0, n\2, binomial(n,n-2*k) * binomial(2*k,k));
for(n=0, 24, print1(a(n),", "))

b(n) = sum(k=0, n\2, binomial(n,n-2*k) * binomial(2*k,k));
for(n=0, 24, print1(b(n),", "))


 