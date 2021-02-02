a(n) = polcoef((1-(2*n-1)*x+(n*x)^2)^n, n);
for(n=0, 20, print1(a(n), ", "))