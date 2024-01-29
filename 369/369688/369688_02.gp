M=26;

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(n,2*k) * binomial(5*k,k) / (4*k+1).
a(n) = sum(k=0, n\2, binomial(n,2*k)*binomial(5*k,k)/(4*k+1));
for(n=0, M, print1(a(n),", "))  

