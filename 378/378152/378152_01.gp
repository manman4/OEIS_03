M=35;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(4*k,n-4*k) * binomial(4*k,k)/(3*k+1).
a(n) = sum(k=0, n\4, binomial(4*k,n-4*k) * binomial(4*k,k)/(3*k+1));
for(n=0, M, print1(a(n),", "))                 

