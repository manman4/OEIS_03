M=20;

a(n) = round(n!*polcoef(cosh(sqrt(n)*log(1-x+x*O(x^n))), n));
for(n=0, M, print1(a(n), ", "));