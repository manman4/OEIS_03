a(n) = sum(k=0, n, k!*stirling(n, k, 2)*(x-1)^(n-k));
T(n, k) = polcoef((1+x)*a(n)^2, k);
for(n=0, 7, for(k=0, 2*(n+0^n)-1, print1(T(n, k), ", ")));


