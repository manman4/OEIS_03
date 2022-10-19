M=11;

T(n, k) = if(n==0, 1, k! * sum(j=1, n, binomial(n-1, j-1) * abs(stirling(j, k, 1)) * T(n-j, k)));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));