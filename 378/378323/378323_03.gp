M=10;

\\ T(n,k) = T(n,k-1) + T(n-1,k+2) + T(n-2,k+2) for n > 1.
T(n, k) = if(n==0, 1, if(n==1, k, if(k==0, 0, T(n,k-1) + T(n-1,k+2) + T(n-2,k+2))));

for(n=0, M, for(k=0, n , print1(T(k,n-k), ", ")));