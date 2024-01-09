a(n) = polcoef(1/(1 - x - n^3*x^2 + x*O(x^n)), n);
for(n=0, 20, print1(a(n), ", "));
