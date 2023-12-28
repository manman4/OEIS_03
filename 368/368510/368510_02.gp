a(n) = polcoef(1/((1-n*x+x*O(x^n)) * (1-x)^(n+1)), n);
for(n=0, 15, print1(a(n), ", "));  
