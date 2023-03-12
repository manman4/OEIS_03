M=18;

a(n) = polcoef(x/(1-2*n*x+(n-1)*n*x^2+x*O(x^n)), n);
for(n=0, M, print1(a(n), ", "));


