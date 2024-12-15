M=25;

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(2*n+k+1,k) * binomial(2*n+k+1,n-3*k)/(2*n+k+1)
a(n) = sum(k=0, n\3, binomial(2*n+k+1, k) * binomial(2*n+k+1, n-3*k)/(2*n+k+1));
for(n=0, M, print1(a(n),", "))

\\ (1/(2*n+1)) * Sum_{k=0..floor(n/3)} binomial(2*n+k,k) * binomial(2*n+k+1,n-3*k).
b(n) = sum(k=0, n\3, binomial(2*n+k, k) * binomial(2*n+k+1, n-3*k)) / (2*n+1);
for(n=0, 100, print1(a(n)-b(n),", "))