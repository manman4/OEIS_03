M=12;

\\ T(n,k) = Sum_{j=1..n} phi(j) * binomial(floor(n/j)+k-1,k). 
T(n, k) = sum(j=1, n, eulerphi(j) * binomial(n\j+k-1, k));
for(n=1, M, for(k=1, n, print1(T(k, n-k+1), ", ")));