\\ a(n) = (1/(n+1)) * [x^n] 1/(1 - x - x/(1 - x))^(n+1).
a(n) = polcoef( 1/(1 - x - x/(1 - x) + x*O(x^n))^(n+1), n)/(n+1);
for(n=0, 25, print1(a(n), ", "))
