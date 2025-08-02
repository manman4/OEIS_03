M=18;

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 2^k * 3^(n-k) * binomial(2*(n+1),k) * binomial(2*n-k,n-k).
a(n) = sum(k=0, n, 2^k * 3^(n-k) * binomial(2*(n+1),k) * binomial(2*n-k,n-k)/(n+1));
for(n=0, M, print1(a(n)", "));