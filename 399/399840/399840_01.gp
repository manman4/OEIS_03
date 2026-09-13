M=100;
\\ T(n,k) = (n*Sum_{j=1..floor(n/k)} binomial(j,n-k*j)/j) mod n.
T(n, k) = (n*sum(j=1, n\k, binomial(j, n-k*j)/j))%n;
for(n=1, M, print1(T(n,3),", "));