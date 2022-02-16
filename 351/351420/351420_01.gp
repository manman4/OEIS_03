M=10;

T(n, k) = if(k==1, (n-1)!, sum(j=1, n, stirling(n, j, 1)*T(j, k-1)));
for(n=1, M, for(k=1, n, print1(T(k, n-k+1),", ")));