\\ T(n,k) = Sum_{j=0..n} (-1)^(k-j) * (2*j+1)^n * binomial(n+1,k-j).
T(n, k) = sum(j=0, n, (-1)^(k-j) * (2*j+1)^n * binomial(n+1, k-j));
for(n=0, 6, for(k=0, 2*n+1, print1(T(n, k), ", ")));


