a(n) = polcoef(x*(1+4*x+x^2)/((1-n*x+x*O(x^n)) * (1-x)^4), n);
for(n=0, 15, print1(a(n), ", "));  
