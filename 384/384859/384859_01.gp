M=10;

\\ Let b(n,k) = 0^n if n*k=0, otherwise b(n,k) = (-1)^n * k * Sum_{j=1..n} (-n+j+k)^(j-1) * binomial(n,j) * b(n-j,3*j). Then A(n,k) = b(n,-k).
b(n, k) = if(n*k==0, 0^n, (-1)^n*k*sum(j=1, n, (-n+j+k)^(j-1)*binomial(n, j)*b(n-j, 3*j)));
a(n, k) = b(n, -k);
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));

