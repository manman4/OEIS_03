\\ a(n) = (2/n) * Sum_{k=0..n-1} k * binomial(4*n-2,n-1-k) for n > 0.
a(n) = if(n==0, 1, (2/n) * sum(k=0, n-1, k * binomial(4*n-2, n-1-k)));
for(n=0, 22, print1(a(n), ", "));

\\ a(n) = (2/n) * Sum_{k=0..floor(n/2)} k * binomial(4*n,n-2*k) for n > 0.
b(n) = if(n==0, 1, (2/n) * sum(k=0, n\2, k * binomial(4*n, n-2*k)));
for(n=0, 22, print1(a(n)-b(n), ", "));

