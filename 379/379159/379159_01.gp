M=24;

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(3*n-9*k+1,k) * binomial(3*n-9*k+1,n-3*k)/(3*n-9*k+1).
a(n) = sum(k=0, n\3, binomial(3*n-9*k+1, k) * binomial(3*n-9*k+1, n-3*k)/(3*n-9*k+1));
for(n=0, M, print1(a(n),", "))

