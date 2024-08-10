\\ a(n) = Sum_{k=0..floor(n/4)} (-1)^k * binomial(n-3*k,k)^2.
a(n) = sum(k=0, n\4, (-1)^k * binomial(n-3*k, k)^2);
for(n=0, 37, print1(a(n),", ")) 
