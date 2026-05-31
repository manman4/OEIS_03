\\ a(n) = Sum_{k=0..n} (k+1) * binomial(4*k+1,k) * binomial(4*n-3*k+1,n-k)/((4*k+1) * (4*n-3*k+1)).
a396416(n) = sum(k=0, n, (k+1) * binomial(4*k+1,k) * binomial(4*n-3*k+1,n-k)/((4*k+1) * (4*n-3*k+1)));
for(n=0, 22, print1(a396416(n),", "));

\\ A(n,k) = Sum_{j=0..n} (j+1) * binomial(4*n-3*j+1,n-j)/(4*n-3*j+1) * A(j,k-1) for k > 0.
