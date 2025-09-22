\\ a(n) = [x^n] (1+x)^(n+3) * (1+3*x)^n.
a(n) = polcoef( (1+x)^(n+3) * (1+3*x)^n + x*O(x^n), n);
for(n=0, 18, print1(a(n),", "));


   