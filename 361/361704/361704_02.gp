M=22;

a(n) = polcoef(polcoef(polcoef(polcoef((1 + w^2 + x^2 + y^2 + z^2 + 1/(w*x*y*z))^n, 0), 0), 0), 0);
for(n=0, M, print1(a(n), ", "));
