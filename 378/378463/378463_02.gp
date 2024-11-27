\\ a(n) = [x^n] 1/(1 - x - x^3/(1 - x))^n.
a(n) = polcoef( 1/(1 - x - x^3/(1 - x) + x*O(x^n))^n, n);
for(n=0, 25, print1(a(n), ", "))

