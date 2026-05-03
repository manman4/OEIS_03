M=30;

\\ a(n) = A378504(n)/(n+1) = (1/6) * (binomial(3*n+2,n) + Sum_{k=0..n+1} 2^(n+1-k) * binomial(3*n+3,k)).
a(n) = 1/6 * (binomial(3*n+2,n) + sum(k=0, n+1, 2^(n+1-k) * binomial(3*n+3,k)));
for(n=0, M, print1(a(n), ", "));
