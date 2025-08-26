\\ a(n) = [x^n] (1+5*x+4*x^2)^(n+1).
a(n) = polcoeff( (1+5*x+4*x^2)^(n+1), n);
for(n=0, 20, print1(a(n),", "))  
   