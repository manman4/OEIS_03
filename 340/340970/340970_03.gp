T(n, k) = polcoef(1/sqrt((1-x)*(1-(4*k+1)*x)+x*O(x^n)), n);
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")))