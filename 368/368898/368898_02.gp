a(n) = polcoef(1/(1 - x - n*x^4 + x*O(x^n)), n);
for(n=0, 31, print1(a(n), ", "));
