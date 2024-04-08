\\ a(n) = [x^n] 1/((1-(n+1)*x) * (1-x)^n).
a(n) = polcoef(1/((1-(n+1)*x) * (1-x)^n + x*O(x^n)), n);
for(n=0, 19, print1(a(n), ", "))