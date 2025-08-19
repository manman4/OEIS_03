\\ T(n,k) = (1/4)^n * Sum_{j=0..n} (4*k+1)^j * binomial(2*j,j) * binomial(2*(n-j),n-j).
T(n, k) = (1/4)^n * sum(j=0, n, (4*k+1)^j * binomial(2*j,j) * binomial(2*(n-j),n-j));
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")))
