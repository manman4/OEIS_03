M=11;

T(n, k) = if(n==0, 1, k! * sum(j=1, n, binomial(n, j) * stirling(j, k, 2) * T(n-j, k)));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));