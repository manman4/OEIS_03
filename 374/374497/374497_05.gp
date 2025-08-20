\\ a(n) = ((n+2)/2) * Sum_{k=0..floor(n/2)} 2^(n-k) * binomial(n+1,n-2*k) * binomial(2*k+1,k).
a(n) = ((n+2)/2) * sum(k=0, n\2, 2^(n-k) * binomial(n+1,n-2*k) * binomial(2*k+1,k));
for(n=0, 20, print1(a(n),", "))  
