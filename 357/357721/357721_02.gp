M=21;

a(n) = round(n!*polcoef(cos(sqrt(n)*log(1+x+x*O(x^n))), n));
for(n=0, M, print1(a(n), ", "));