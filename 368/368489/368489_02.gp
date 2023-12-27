a(n) = polcoef(1/((1-x+x*O(x^n)) * (1-n*x)^(n+1)), n);
for(n=0, 15, print1(a(n), ", "));  
