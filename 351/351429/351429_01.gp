M=10;

T(n, k) = if(k==0, (-1)^n*n!, sum(j=0, n, stirling(n, j, 2)*T(j, k-1)));
for(n=0, M, for(k=0, n, print1(T(k, n-k),", ")));