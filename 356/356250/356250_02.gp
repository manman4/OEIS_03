M=11;

T(n, k) = if(k==0, n, sum(j=1, n, j^k*sumdiv(j, d, 1-(1-1/d)^k)));
for(n=1, M, for(k=1, n, print1(T(k, n-k), ", ")));