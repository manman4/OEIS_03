\\ a(n) = (1/(4*n-1)) * Sum_{k=0..n} (4*k-1) * binomial(4*n-1,n-k).
a(n) = sum(k=0, n, (4*k-1) * binomial(4*n-1, n-k))/(4*n-1);
for(n=0, 22, print1(a(n), ", "));



