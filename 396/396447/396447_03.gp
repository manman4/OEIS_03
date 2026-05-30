\\ A(n,0) = 0^n; A(n,k) = Sum_{j=0..n} (2*j+1) * binomial(4*n-2*j+1,n-j)/(4*n-2*j+1) * A(j,k-1) for k > 0.
a(n,k) = if(k==0, 0^n, sum(j=0, n, (2*j+1) * binomial(4*n-2*j+1, n-j)/(4*n-2*j+1) * a(j,k-1)));
for(n=0, 10, for(k=0, n, print1(a(k, n-k), ", ")));

