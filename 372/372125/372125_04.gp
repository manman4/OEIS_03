\\ a(n) = (1/(n+1)) * [x^n] 1/(1 - x*(1 + 4*x)^(1/2))^(n+1).
a(n) = polcoeff(1/(1 - x*(1 + 4*x + x*O(x^n))^(1/2) )^(n+1), n) / (n+1);
for(n=0, 20, print1(a(n), ", "))
