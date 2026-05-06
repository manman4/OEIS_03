\\ a(n) = Sum_{k=0..n} 3^k * binomial(k+2,2) * binomial(4*n+11,n-k).
a(n) = sum(k=0, n, 3^k * binomial(k+2,2) * binomial(4*n+11,n-k));
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} 4^k * binomial(k+2,2) * binomial(4*n+8-k,n-k).
b(n) = sum(k=0, n, 4^k * binomial(k+2,2) * binomial(4*n+8-k,n-k));
for(n=0, 30, print1(a(n)-b(n), ", "));



\\ a(0) = 1; a(n) = (1/n) * Sum_{k=0..n-1} (4*k+24) * 3^k * binomial(k+3,3) * binomial(4*n+11,n-1-k).
c(n) = if(n==0, 1, (1/n) * sum(k=0, n-1, (4*k+24) * 3^k * binomial(k+3,3) * binomial(4*n+11,n-1-k)));
for(n=0, 30, print1(a(n)-c(n), ", "));

\\ a(0) = 1; a(n) = (1/n) * Sum_{k=0..n-1} (3*k+24) * 4^k * binomial(k+3,3) * binomial(4*n+7-k,n-1-k).
d(n) = if(n==0, 1, (1/n) * sum(k=0, n-1, (3*k+24) * 4^k * binomial(k+3,3) * binomial(4*n+7-k,n-1-k)));
for(n=0, 30, print1(a(n)-d(n), ", "));