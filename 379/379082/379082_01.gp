M=25;

\\ a(n) = 2 * Sum_{k=0..floor(n/3)} binomial(2*n+k+2,k) * binomial(2*n+k+2,n-3*k)/(2*n+k+2) 
a(n) = 2 * sum(k=0, n\3, binomial(2*n+k+2, k) * binomial(2*n+k+2, n-3*k)/(2*n+k+2));
for(n=0, M, print1(a(n),", "))

\\ (1/(n+1)) * Sum_{k=0..floor(n/3)} binomial(2*n+k+1,k) * binomial(2*n+k+2,n-3*k).
b(n) = sum(k=0, n\3, binomial(2*n+k+1, k) * binomial(2*n+k+2, n-3*k)) / (n+1);
for(n=0, 100, print1(a(n)-b(n),", "))