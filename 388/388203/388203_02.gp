\\ a(n) = [x^n] (1-2*x)^n/(1-3*x)^(n+4).
a(n) = polcoef( (1-2*x)^n/(1-3*x + x*O(x^n))^(n+4), n);
for(n=0, 19, print1(a(n),", "));


   