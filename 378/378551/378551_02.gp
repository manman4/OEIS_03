\\ a(n) = [x^n] 1/(1 - 4*x/(1-x))^(n/2).
a(n) = polcoef( 1/(1 - 4*x/(1-x) + x*O(x^n))^(n/2), n);
for(n=0, 20, print1(a(n), ", "));

 