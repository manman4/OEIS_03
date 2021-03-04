T(n, k) = sum(j=0, n, (2*k-2)^j*binomial(n+1+j, 2*j+1));
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")))