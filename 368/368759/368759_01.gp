T(n, k) = n!*(1+sum(j=0, n, j^k/j!));
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")));  
