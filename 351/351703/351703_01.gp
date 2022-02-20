M=11;

T(n, k) = if(n==0, 1, binomial(n, k)*sum(j=0, n-k, binomial(n-k, j)*T(j, k)));
for(n=0, M, for(k=0, n-1, print1(T(k, n-k), ", ")))