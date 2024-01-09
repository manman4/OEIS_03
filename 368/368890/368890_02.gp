a(n) = polcoef(1/(1 - n^3*x - x^2 + x*O(x^n)), n);
for(n=0, 20, print1(a(n), ", "));
