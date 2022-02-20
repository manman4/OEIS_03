M=10;

T(n, k) = if(n==0, 1, -k*n*sum(j=0, n-1, binomial(n-1, j)*T(j, k)));
for(n=0, M, for(k=0, n, print1(T(k, n-k),", ")));