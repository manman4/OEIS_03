M=42;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(k,n-4*k) * binomial(3*k,k)/(2*k+1).
a(n) = sum(k=0, n\4, binomial(k,n-4*k) * binomial(3*k,k)/(2*k+1));
for(n=0, M, print1(a(n), ", "));