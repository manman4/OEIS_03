\\ a(n) = [x^n] 1/((1-x-n*x^2) * (1-x)^n).
a(n) = polcoef(1/((1-x-n*x^2) * (1-x)^n + x*O(x^n)), n);
for(n=0, 19, print1(a(n), ", "))