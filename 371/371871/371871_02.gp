\\ a(n) = [x^n] 1/((1-x^3) * (1-x)^(n-1)).
a(n) = polcoef(1/((1-x^3) * (1-x)^(n-1) +x*O(x^n)), n);
for(n=0, 30, print1(a(n), ", "))