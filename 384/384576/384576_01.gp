M=22;

\\ A(n,0) = 0^n; A(n,k) = k * Sum_{j=0..n} binomial(2*n-2*j+k,j)/(2*n-2*j+k) * A(n-j,j).
a(n, k=2) = if(k==0, 0^n, k*sum(j=0, n, binomial(2*n-2*j+k, j)/(2*n-2*j+k)*a(n-j, j)));
for(n=0, M, print1(a(n),", "));

