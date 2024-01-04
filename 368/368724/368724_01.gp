T(n, k) = n!*sum(j=0, n, (-1)^(n-j)*j^k/j!);
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")));  
