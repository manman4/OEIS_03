\\ T(0,k) = 0^k; T(n,k) = k*T(n-1,k) + n^k.
T(n, k) = if(n==0, 0^k, k*T(n-1, k) + n^k);
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")));  
