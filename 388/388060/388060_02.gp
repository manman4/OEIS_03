\\ a(n) = [x^n] (1-3*x)^n/(1-4*x)^(n+2).
a(n) = polcoef( (1-3*x)^n/(1-4*x + x*O(x^n))^(n+2), n);
for(n=0, 19, print1(a(n),", "));


   