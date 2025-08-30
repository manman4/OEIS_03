\\ T(n,k) = Sum_{j=0..floor(n/2)} (k*(k+1))^j * (2*k+1)^(n-2*j) * binomial(n,2*j) * binomial(2*j,j).
T(n,k) = sum(j=0, n\2, (k*(k+1))^j * (2*k+1)^(n-2*j) * binomial(n,2*j) * binomial(2*j,j));
for(n=0, 10, for(k=0, n, print1(T(k,n-k),", ")));
