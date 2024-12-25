\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} 2^(n-2*k) * binomial(n+1,k) * binomial(5*(n+1)-k,n-2*k).
a(n) = (1/(n+1)) * sum(k=0, n\2, 2^(n-2*k) * binomial(n+1,k) * binomial(5*(n+1)-k,n-2*k));
for(n=0, 17, print1(a(n),", "))

