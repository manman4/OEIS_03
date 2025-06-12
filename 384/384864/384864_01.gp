M=11;

\\ Let b(n,k) = 0^n if n*k=0, otherwise b(n,k) = (-1)^n * k * Sum_{j=1..n} binomial(-2*n+3*j+k-1,j-1) * b(n-j,j)/j. Then A(n,k) = b(n,-k).
b(n, k) = if(n*k==0, 0^n, (-1)^n*k*sum(j=1, n, binomial(-2*n+3*j+k-1,j-1)*b(n-j,j)/j));
a(n, k) = b(n, -k);
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));

