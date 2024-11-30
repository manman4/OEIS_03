\\ a(n) = [x^n] 1/(1 - x*(1 + 4*x)^(1/2))^n.
a(n) = polcoef( 1/(1 - x*(1 + 4*x + x*O(x^n))^(1/2))^n, n);
for(n=0, 20, print1(a(n), ", "));

 