\\ a(n) = (1/(n-1)) * Sum_{k=0..n} (k-1) * binomial(k+3,3) * binomial(4*n-4,n-k) for n > 1.
a(n) = if(n<2, 0^n, (1/(n-1)) * sum(k=0, n, (k-1) * binomial(k+3, 3) * binomial(4*n-4, n-k)));
for(n=0, 21, print1(a(n), ", "));

\\ a(n) = (8/n) * Sum_{k=0..n-1} binomial(k+3,4) * binomial(4*n-5,n-1-k) for n > 0.
b(n) = if(n==0, 1, (8/n) * sum(k=0, n-1, binomial(k+3,4) * binomial(4*n-5, n-1-k)));
for(n=0, 21, print1(a(n)-b(n), ", "));
