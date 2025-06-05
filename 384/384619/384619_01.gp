M=10;

\\ A(n,0) = 0^n; A(n,k) = k * Sum_{j=0..n} binomial(n+j+k,j)/(n+j+k) * A(n-j,2*j).
a(n, k) = if(k==0, 0^n, k*sum(j=0, n, binomial(n+j+k, j)/(n+j+k)*a(n-j, 2*j)));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));

