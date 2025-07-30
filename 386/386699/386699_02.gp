\\ a(n) = [x^n] 1/((1-3*x) * (1-x)^(4*n)).
a(n) = polcoef(1/( (1-3*x) * (1-x)^(4*n) + x*O(x^n) ), n);
for(n=0, 20, print1(a(n), ", "));


