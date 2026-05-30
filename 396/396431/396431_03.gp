\\ A(n,0) = 0^n; A(n,k) = 3 * Sum_{j=0..n} (j+1) * binomial(4*n-j+3,n-j)/(4*n-j+3) * A(j,k-1) for k > 0.
a(n,k) = if(k==0, 0^n, 3 * sum(j=0, n, (j+1) * binomial(4*n-j+3, n-j)/(4*n-j+3) * a(j,k-1)));
for(n=0, 10, for(k=0, n, print1(a(k, n-k), ", ")));
