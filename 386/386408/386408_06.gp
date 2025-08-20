\\ A(n,k) = Sum_{j=0..floor(n/2)} k^(2*j) * (2*k+1)^(n-2*j) * binomial(n,2*j) * Catalan(j).
catalan(n) = binomial(2*n, n)/(n+1);
a(n,k) = sum(j=0, n\2, k^(2*j) * (2*k+1)^(n-2*j) * binomial(n, 2*j) * catalan(j));
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));
