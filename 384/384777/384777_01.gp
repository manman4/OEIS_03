M=9;

\\ A(n,0) = 0^n; A(n,k) = k * Sum_{j=0..n} 2^(n-j) * binomial(n+k,j)/(n+k) * A(n-j,j).
a(n, k) = if(k==0, 0^n, k*sum(j=0, n, 2^(n-j)*binomial(n+k, j)/(n+k)*a(n-j, j)));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));

