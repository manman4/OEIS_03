T(n, k) = sum(j=0, n, j^k*(n-j)!*binomial(n, j)^2);
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")))