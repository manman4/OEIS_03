a(n) = polcoef(1/((1-x+x*O(x^n)) * (1-n*x)^n), n);
for(n=0, 16, print1(a(n), ", "));  
