\\ a(n) = (1/(3*n-2)) * Sum_{k=0..n} (k+1) * (3*k-2) * binomial(3*n-2,n-k).
a(n) = sum(k=0, n, (k+1) * (3*k-2) * binomial(3*n-2, n-k))/(3*n-2);
for(n=0, 23, print1(a(n), ", "));

\\ a(n) = (4/n) * Sum_{k=0..n-1} binomial(k+1,2) * binomial(3*n-3,n-1-k) for n > 0.
b(n) = if(n==0, 1, (4/n) * sum(k=0, n-1, binomial(k+1,2) * binomial(3*n-3, n-1-k)));
for(n=0, 50, print1(a(n)-b(n), ", "));
