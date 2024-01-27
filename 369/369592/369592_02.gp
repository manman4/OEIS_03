M=41;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/4)} (n-4*k+1) * binomial(n+1,k).
a(n) = (1/(n+1)) * sum(k=0, n\4, (n-4*k+1)*binomial(n+1,k));
for(n=0, M, print1(a(n),", "))  
