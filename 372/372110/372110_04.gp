\\ a(n) = (1/(n+1)) * [x^n] 1/(1 - 9*x/(1-x))^((n+1)/3).
a(n) = polcoeff(1/(1 - 9*x/(1-x) + x*O(x^n) )^((n+1)/3), n) / (n+1);
for(n=0, 20, print1(a(n), ", "))