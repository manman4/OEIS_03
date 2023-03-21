M=28;

a(n) = polcoef(polcoef((1 + x^3 + y^3 + 1/(x*y))^n, 0), 0);
for(n=0, M, print1(a(n), ", "));
