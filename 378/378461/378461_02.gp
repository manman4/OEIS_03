\\ a(n) = [x^n] 1/(1 - x - x/(1 - x)^2)^n.
a(n) = polcoef( 1/(1 - x - x/(1 - x)^2 + x*O(x^n))^n, n); 
for(n=0, 25, print1(a(n), ", "))

