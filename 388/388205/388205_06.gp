\\ a(n) = [x^n] (1+x)^(n+2) * (4+x)^n.
a(n) = polcoef( (1+x)^(n+2) * (4+x)^n + x*O(x^n), n);
for(n=0, 18, print1(a(n),", "));


   