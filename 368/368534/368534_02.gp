a(n) = polcoef(x/((1-n*x+x*O(x^n)) * (1-x)^3), n);
for(n=0, 15, print1(a(n), ", "));  
