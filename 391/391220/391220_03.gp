\\ a(n) = (1/(2*n-1)) * Sum_{k=0..n} (k+1) * (2*k-1) * binomial(4*n-2,n-k).
a(n) = sum(k=0, n, (k+1) * (2*k-1) * binomial(4*n-2, n-k))/(2*n-1);
for(n=0, 22, print1(a(n), ", "));

\\ a(n) = (4/n) * Sum_{k=0..n-1} binomial(k+1,2) * binomial(4*n-3,n-1-k) for n > 0.
b(n) = if(n==0, 1, (4/n) * sum(k=0, n-1, binomial(k+1,2) * binomial(4*n-3, n-1-k)));
for(n=0, 22, print1(a(n)-b(n), ", "));
