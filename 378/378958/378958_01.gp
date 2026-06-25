M=25;

\\ a(n) = (1/n) * Sum_{k=0..n} (-1)^k * binomial(n,k) * binomial(3*n-4*k,n-k-1) for n > 0.
C(n, k) = if(k>=0, binomial(n, k));
a(n) = if(n==0, 1, sum(k=0, n, (-1)^k*C(n, k)*C(3*n-4*k, n-k-1))/n);
for(n=0, M, print1(a(n), ", "))
