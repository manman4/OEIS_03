T(n, k) = if(n==0, 0^k, n^k - n * T(n-1,k));
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")));  
