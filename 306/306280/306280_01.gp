\\ a(n) = [x^n] (1+x)^n/(1-x)^(n^2+1).
a(n) = polcoef( (1+x)^n/(1-x + x*O(x^n))^(n^2+1), n);
for(n=0, 20, print1(a(n),", "));


   