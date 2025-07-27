\\ a(n) = Sum_{k=0..n-1} binomial(3*k+1,k) * binomial(3*n-3*k,n-k-1).
a(n) = sum(k=0, n-1, binomial(3*k+1, k) * binomial(3*n-3*k, n-k-1));
for(n=0, 23, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n-1} binomial(3*k,k) * binomial(3*n-3*k+1,n-k-1).
b(n) = sum(k=0, n-1, binomial(3*k, k) * binomial(3*n-3*k+1, n-k-1));
for(n=0, 50, print1(a(n)-b(n), ", "));
