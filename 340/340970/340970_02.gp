T(n, k) = polcoef((1+(2*k+1)*x+(k*x)^2)^n, n);
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")))