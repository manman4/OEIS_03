\\ a(n) = [x^n] 1/(1 - x*(1 + 9*x)^(1/3))^n.
a(n) = polcoef( 1/(1 - x*(1 + 9*x + x*O(x^n))^(1/3))^n, n);
for(n=0, 20, print1(a(n), ", "));

 