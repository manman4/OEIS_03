M=10;

\\ A(n,0) = 0^n; A(n,k) = k * Sum_{j=0..n} binomial(2*n-j+k,j)/(2*n-j+k) * A(n-j,j).
a(n, k) = if(k==0, 0^n, k*sum(j=0, n, binomial(2*n-j+k, j)/(2*n-j+k)*a(n-j, j)));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));

