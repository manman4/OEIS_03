T(n, k) = if(n==0, 1, n*sum(j=0, n, (2*k-2)^j*binomial(n+j, 2*j)/(n+j)));
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")))