\\ a(n) = [x^n] (1+2*x)^n/(1-x)^(n+5).
a(n) = polcoef( (1+2*x)^n/(1-x + x*O(x^n))^(n+5), n);
for(n=0, 19, print1(a(n),", "));


   