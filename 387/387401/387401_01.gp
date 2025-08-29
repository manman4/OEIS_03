\\ a(n) = Sum_{k=0..floor(n/2)} 2^(n-k) * binomial(n+1,n-2*k) * binomial(2*k+1,k).
a(n) = sum(k=0, n\2, 2^(n-k) * binomial(n+1,n-2*k) * binomial(2*k+1,k));
for(n=0, 25, print1(a(n),", "))
