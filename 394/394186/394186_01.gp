\\ a(n) = Sum_{k=0..n} binomial(n+k,2*k) * (binomial(n+2*k,k) - binomial(n+2*k,k-1)).
a(n) = sum(k=0, n, binomial(n+k,2*k) * (binomial(n+2*k,k) - binomial(n+2*k,k-1)));
for(n=0, 21, print1(a(n), ", "));

\\ T(n,k) = Sum_{j=0..floor(n/2)} binomial(n-j,j) * (binomial(n+k-2*j,k-j) - binomial(n+k-2*j,k-j-1)). 
T(n,k) = sum(j=0, n\2, binomial(n-j,j) * (binomial(n+k-2*j,k-j) - binomial(n+k-2*j,k-j-1)));
\\ a(n) = A071945(2*n,n).
for(n=0, 50, print1(T(2*n,n)-a(n), ", "));

\\ a(n) = Sum_{k=0..n} C(n + 2*k, 3*k) * C(3*k, 2*k).
b(n) = sum(k=0, n, binomial(n + 2*k, 3*k) * binomial(3*k, 2*k));
\\ a(n) = Sum_{k=0..n} binomial(n+k,2*k) * binomial(n+2*k,k-1).
c(n) = sum(k=0, n, binomial(n+k,2*k) * binomial(n+2*k,k-1));
\\ a(n) = A243116(n) - A397383(n).
for(n=0, 50, print1(b(n)-c(n)-a(n), ", "));
