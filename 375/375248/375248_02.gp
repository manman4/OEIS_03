\\ a(n) = (binomial(n+5,3)/10) * Sum_{k=0..floor(n/2)} binomial(n+2,n-2*k) * binomial(2*k+2,k).
a(n) = (binomial(n+5,3)/10) * sum(k=0, n\2, binomial(n+2,n-2*k) * binomial(2*k+2,k));
for(n=0, 24, print1(a(n),", "))

b(n) = sum(k=0, n\2, binomial(n+2,n-2*k) * binomial(2*k+2,k));
for(n=0, 24, print1(b(n),", "))


 