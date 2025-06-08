M=9;

\\ A(n,0) = 0^n; A(n,k) = k * Sum_{j=0..n} k^(j-1) * binomial(n,j) * A(n-j,2*j).
\\ A(n,0) = 0^n; A(n,k) = Sum_{j=0..n} k^j * binomial(n,j) * A(n-j,2*j).
b(n, k) = if(k==0, 0^n, sum(j=0, n, k^j*binomial(n,j)*b(n-j, 2*j)));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));

\\ A(n,k) = k * (2*n+k)^(n-1) for n > 0.
a(n, k) = if(n==0, 1, k*(2*n+k)^(n-1));
for(n=0, M, for(k=0, n, print1(a(k, n-k)-b(k, n-k),", ")));