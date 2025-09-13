\\ a(n) = [x^n] (1-2*x)^n/(1-3*x)^(2*n+1).
a(n) = polcoef( (1-2*x)^n/(1-3*x + x*O(x^n))^(2*n+1), n);
for(n=0, 19, print1(a(n),", "));


   