\\ T(n,k) = (-1)^n * Sum_{j=0..n} (1/(2*(2*k+1)))^(n-2*j) * binomial(-1/2,j) * binomial(j,n-j).
T(n,k) = (-1)^n * sum(j=0, n, (1/(2*(2*k+1)))^(n-2*j) * binomial(-1/2,j) * binomial(j,n-j));
for(n=0, 10, for(k=0, n, print1(T(k,n-k),", ")));
