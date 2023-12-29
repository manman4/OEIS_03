a(n) = polcoef(n*x/((1-x+x*O(x^n)) * (1-n*x)^4), n);
for(n=0, 15, print1(a(n), ", "));  
