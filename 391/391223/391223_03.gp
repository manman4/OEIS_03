\\ a(n) = (1/(2*n-3)) * Sum_{k=0..n} (2*k-3) * binomial(k+2,2) * binomial(2*n-3,n-k).
a(n) = sum(k=0, n, (2*k-3) * binomial(k+2,2) * binomial(2*n-3, n-k))/(2*n-3);
for(n=0, 26, print1(a(n), ", "));

\\ a(n) = (6/n) * Sum_{k=0..n-1} binomial(k+2,3) * binomial(2*n-4,n-1-k) for n > 0.
b(n) = if(n==0, 1, (6/n) * sum(k=0, n-1, binomial(k+2,3) * binomial(2*n-4, n-1-k)));
for(n=0, 50, print1(a(n)-b(n), ", "));
