M=13;

\\ a(n) = Sum_{k=0..n} 2^k * binomial(2*n-1,k+n-1) * Stirling2(k+n-1,n-1) for n > 0.
a(n) = if(n==0, 1, sum(k=0, n, 2^k*binomial(2*n-1,k+n-1)*stirling(k+n-1,n-1,2)));
for(n=0, M, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} (-2)^k * (2*n-1)^(n-k) * binomial(2*n-1,k+n-1) * Stirling2(k+n-1,n-1) for n > 0.
b(n) = if(n==0, 1, sum(k=0, n, (-2)^k*(2*n-1)^(n-k)*binomial(2*n-1,k+n-1)*stirling(k+n-1,n-1,2)));
for(n=0, 50, print1(a(n)-b(n), ", "));