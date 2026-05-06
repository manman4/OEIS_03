\\ a(n) = Sum_{k=0..n-1} 5^(n-k-1) * binomial(6*n-1,k).
a(n) = sum(k=0, n-1, 5^(n-k-1) * binomial(6*n-1,k));
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n-1} 6^(n-k-1) * binomial(5*n+k-1,k).
b(n) = sum(k=0, n-1, 6^(n-k-1) * binomial(5*n+k-1,k));
for(n=0, 30, print1(a(n)-b(n), ", "));



\\ a(n+1) = (1/n) * Sum_{k=0..n-1} (k+1) * (6*k+16) * 5^k * binomial(6*n+5,n-1-k) for n > 0.
c(n) = if(n<2, n, (1/(n-1)) * sum(k=0, n-2, (k+1) * (6*k+16) * 5^k * binomial(6*n-1,n-2-k)));
for(n=0, 30, print1(a(n)-c(n), ", "));

\\ a(n+1) = (1/n) * Sum_{k=0..n-1} (k+1) * (5*k+16) * 6^k * binomial(6*n+3-k,n-1-k) for n > 0. 
d(n) = if(n<2, n, (1/(n-1)) * sum(k=0, n-2, (k+1) * (5*k+16) * 6^k * binomial(6*n-3-k,n-2-k)));
for(n=0, 30, print1(a(n)-d(n), ", "));