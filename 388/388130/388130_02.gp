\\ a(n) = [x^n] (1+x)^n/(1-x)^(3*n+2).
a(n) = polcoef( (1+x)^n/(1-x + x*O(x^n))^(3*n+2), n);
for(n=0, 20, print1(a(n),", "));


   