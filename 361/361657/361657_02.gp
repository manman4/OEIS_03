M=27;

a(n) = polcoef(polcoef((1 + x^2 + y^2 + 1/(x*y))^n, 0), 0);
for(n=0, M, print1(a(n), ", "));
