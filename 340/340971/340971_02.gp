a(n) = polcoef(1/sqrt((1-x)*(1-(4*n+1)*x)+x*O(x^n)), n);
for(n=0, 20, print1(a(n), ", "))