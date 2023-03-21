M=29;

a(n) = polcoef(polcoef((1 + x^4 + y^4 + 1/(x*y))^n, 0), 0);
for(n=0, M, print1(a(n), ", "));
