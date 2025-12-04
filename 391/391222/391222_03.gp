\\ a(n) = (1/(n-1)) * Sum_{k=0..n} (k-1) * (k+1) * binomial(2*n-2,n-k) for n > 1.
a(n) = if(n<2, 0^n, (1/(n-1)) * sum(k=0, n, (k-1) * (k+1) * binomial(2*n-2, n-k)));
for(n=0, 26, print1(a(n), ", "));

\\ a(n) = (4/n) * Sum_{k=0..n-1} binomial(k+1,2) * binomial(2*n-3,n-1-k) for n > 0.
b(n) = if(n==0, 1, (4/n) * sum(k=0, n-1, binomial(k+1,2) * binomial(2*n-3, n-1-k)));
for(n=0, 50, print1(a(n)-b(n), ", "));
