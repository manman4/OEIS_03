M=25;

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(3*n-12*k+1,k) * binomial(3*n-12*k+1,n-4*k)/(3*n-12*k+1).
a(n) = sum(k=0, n\4, binomial(3*n-12*k+1, k) * binomial(3*n-12*k+1, n-4*k)/(3*n-12*k+1));
for(n=0, M, print1(a(n),", "))

