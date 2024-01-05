a337085(n, k) = n!*sum(j=0, n, j^k/j!);
for(n=0, 10, for(k=0, n, print1(a337085(k, n-k), ", ")));  

T(n, k) = n! + a337085(n, k);
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")));  