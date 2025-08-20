\\ a(n) = ((n+2)/2) * Sum_{k=0..floor(n/2)} 4^k * 5^(n-2*k) * binomial(n+1,n-2*k) * binomial(2*k+1,k).
a(n) = ((n+2)/2) * sum(k=0, n\2, 4^k * 5^(n-2*k) * binomial(n+1,n-2*k) * binomial(2*k+1,k));
for(n=0, 25, print1(a(n),", "))  
