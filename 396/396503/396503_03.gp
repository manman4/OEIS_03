\\ A(n,0) = 0^n; A(n,k) = 2 * Sum_{j=0..n} (j+1) * (3*n-j+2)^(n-j-1) * binomial(n,j) * A(j,k-1) for k > 0.
a(n,k) = if(k==0, 0^n, 2 * sum(j=0, n, (j+1) * (3*n-j+2)^(n-j-1) * binomial(n,j) * a(j,k-1)));
for(n=0, 9, for(k=0, n, print1(a(k, n-k), ", ")));
