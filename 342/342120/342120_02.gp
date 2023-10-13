T(n, k) = sum(j=0, n, k^j*binomial(j, n-j));
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")))