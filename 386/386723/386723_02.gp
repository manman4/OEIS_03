M=17;

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 2^k * binomial(4*(n+1),k) * binomial(4*n-k+2,n-k).
a(n) = sum(k=0, n, 2^k * binomial(4*(n+1),k) * binomial(4*n-k+2,n-k)/(n+1));
for(n=0, M, print1(a(n)", "))