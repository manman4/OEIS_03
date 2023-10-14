T(n, k) = polcoef(serlaplace(exp(k*n*x/(1-x+x*O(x^n)))/(1-x+x*O(x^n))), n);
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")))