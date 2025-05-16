M=13;

\\ a(n) = Sum_{k=0..n} 2^k * binomial(3*n-1,k+2*n-1) * Stirling2(k+2*n-1,2*n-1) for n > 0.
a(n) = if(n==0, 1, sum(k=0, n, 2^k*binomial(3*n-1,k+2*n-1)*stirling(k+2*n-1,2*n-1,2)));
for(n=0, M, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} (-2)^k * (4*n-1)^(n-k) * binomial(3*n-1,k+2*n-1) * Stirling2(k+2*n-1,2*n-1) for n > 0. 
b(n) = if(n==0, 1, sum(k=0, n, (-2)^k*(4*n-1)^(n-k)*binomial(3*n-1,k+2*n-1)*stirling(k+2*n-1,2*n-1,2)));
for(n=0, 50, print1(a(n)-b(n), ", "));