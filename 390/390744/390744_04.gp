M=22;
\\ a(0) = 1; a(n) = 8 * Sum_{k=0..floor(n/2)} k * binomial(5*n-2*k,n-2*k)/(5*n-2*k).
a(n) = if(n==0, 1, 8 * sum(k=0, n\2, k * binomial(5*n-2*k, n-2*k)/(5*n-2*k)));
for(n=0, M, print1(a(n),", "));

\\ a(0) = 1; a(n) = (2/n) * Sum_{k=0..floor(n/2)} k * binomial(5*n-2*k-1,n-2*k).
b(n) = if(n==0, 1, (2/n) * sum(k=0, n\2, k * binomial(5*n-2*k-1, n-2*k)));
for(n=0, 100, print1(a(n)-b(n),", "));