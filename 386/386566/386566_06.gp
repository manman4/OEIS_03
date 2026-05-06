\\ a(n) = Sum_{k=0..n-1} 4^(n-k-1) * binomial(5*n,k).
a(n) = sum(k=0, n-1, 4^(n-k-1) * binomial(5*n,k));
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n-1} 5^(n-k-1) * binomial(4*n+k,k).
b(n) = sum(k=0, n-1, 5^(n-k-1) * binomial(4*n+k,k));
for(n=0, 30, print1(a(n)-b(n), ", "));



\\ a(n+1) = (1/n) * Sum_{k=0..n-1} (k+1) * (5*k+14) * 4^k * binomial(5*n+5,n-1-k) for n > 0.
c(n) = if(n<2, n, (1/(n-1)) * sum(k=0, n-2, (k+1) * (5*k+14) * 4^k * binomial(5*n,n-2-k)));
for(n=0, 30, print1(a(n)-c(n), ", "));

\\ a(n+1) = (1/n) * Sum_{k=0..n-1} (k+1) * (4*k+14) * 5^k * binomial(5*n+3-k,n-1-k) for n > 0. 
d(n) = if(n<2, n, (1/(n-1)) * sum(k=0, n-2, (k+1) * (4*k+14) * 5^k * binomial(5*n-2-k,n-2-k)));
for(n=0, 30, print1(a(n)-d(n), ", "));