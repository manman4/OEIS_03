M=6;

T(n, k) = if(k==0, 1, if(k==n, prod(j=1, n, j^j), T(n-1, k)+n^n*T(n-1, k-1)));
for(n=0, M, for(k=0, n, print1(T(n, k), ", ")));