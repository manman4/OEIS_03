M=20;
\\ T(n,k) = (n*Sum_{j=1..floor(n/k)} binomial(j,n-k*j)/j) mod n.
T(n, k) = (n*sum(j=1, n\k, binomial(j, n-k*j)/j))%n;
for(n=2, M, for(k=1, n\2, print1(T(n,k),", ")));