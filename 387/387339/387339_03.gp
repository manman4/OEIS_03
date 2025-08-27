\\ a(n) = Sum_{k=0..floor(n/2)} 3^k * 4^(n-2*k) * binomial(n+2,n-2*k) * binomial(2*k+2,k).
a(n) = sum(k=0, n\2, 3^k * 4^(n-2*k) * binomial(n+2,n-2*k) * binomial(2*k+2,k));
for(n=0, 20, print1(a(n),", "))
