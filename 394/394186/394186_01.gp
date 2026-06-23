\\ a(n) = Sum_{k=0..n} binomial(n+k,2*k) * (binomial(n+2*k,k) - binomial(n+2*k,k-1)).
a(n) = sum(k=0, n, binomial(n+k,2*k) * (binomial(n+2*k,k) - binomial(n+2*k,k-1)));
for(n=0, 21, print1(a(n), ", "));

\\ T(n,k) = Sum_{j=0..floor(n/2)} binomial(n-j,j) * (binomial(n+k-2*j,k-j) - binomial(n+k-2*j,k-j-1)). 
T(n,k) = sum(j=0, n\2, binomial(n-j,j) * (binomial(n+k-2*j,k-j) - binomial(n+k-2*j,k-j-1)));
\\ a(n) = A071945(2*n,n).
for(n=0, 50, print1(a(n)-T(2*n,n), ", "));
