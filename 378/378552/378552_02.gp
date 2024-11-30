\\ a(n) = [x^n] 1/(1 - 9*x/(1-x))^(n/3).
a(n) = polcoef( 1/(1 - 9*x/(1-x) + x*O(x^n))^(n/3), n);
for(n=0, 20, print1(a(n), ", "));

 