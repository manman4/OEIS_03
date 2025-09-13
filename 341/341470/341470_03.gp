\\ T(n,k) = [x^n] (1-x)^n/(1-2*x)^(k*n+1).
T(n,k) = polcoef( (1-x)^n/(1-2*x + x*O(x^n))^(k*n+1), n);
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")));


   