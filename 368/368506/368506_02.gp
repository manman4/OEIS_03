T(n, k) = polcoef(1/((1-k*x+x*O(x^n)) * (1-x)^k), n);
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")));  
