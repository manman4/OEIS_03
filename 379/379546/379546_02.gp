\\ (1/(n+1)) * Sum_{k=0..floor(n/2)} 2^(n-2*k) * binomial(n+1,k) * binomial(4*(n+1)-k,n-2*k).
a(n) = (1/(n+1)) * sum(k=0, n\2, 2^(n-2*k) * binomial(n+1,k) * binomial(4*(n+1)-k,n-2*k));
for(n=0, 18, print1(a(n),", "))

a371669(n) = if(n==0, 1, sum(k=0, (n-1)\2, 2^(n-2*k)*binomial(n, k)*binomial(4*n-k, n-1-2*k))/n);
\\ a(n) = A371669(n+1)/2
for(n=0, 100, print1(a(n)-a371669(n+1)/2, ", "))