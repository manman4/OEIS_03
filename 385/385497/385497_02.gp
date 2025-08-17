\\ a(n) = [x^n] (1+x)^(6*n+1)/(1-x).
a(n) = polcoef( (1+x)^(6*n+1)/(1-x + x*O(x^n)), n);
for(n=0, 27, print1(a(n), ", "));

