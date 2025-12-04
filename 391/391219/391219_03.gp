\\ a(n) = (1/(n-1)) * Sum_{k=0..n} (k-1) * binomial(k+2,2) * binomial(3*n-3,n-k) for n > 1.
a(n) = if(n<2, 0^n, (1/(n-1)) * sum(k=0, n, (k-1) * binomial(k+2,2) * binomial(3*n-3, n-k)));
for(n=0, 23, print1(a(n), ", "));

\\ a(n) = (6/n) * Sum_{k=0..n-1} binomial(k+2,3) * binomial(3*n-4,n-1-k) for n > 0.
b(n) = if(n==0, 1, (6/n) * sum(k=0, n-1, binomial(k+2,3) * binomial(3*n-4, n-1-k)));
for(n=0, 50, print1(a(n)-b(n), ", "));
