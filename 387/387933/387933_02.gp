\\ a(n) = [x^n] (1+2*x)^n/(1-x)^(4*n+1).
a(n) = polcoef( (1+2*x)^n/(1-x + x*O(x^n))^(4*n+1), n);
for(n=0, 20, print1(a(n),", "));


   