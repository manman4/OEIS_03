default(parisize, 120000000)

M=22;

a(n) = polcoef(polcoef(polcoef(polcoef((1 + w^4 + x^4 + y^4 + z^4 + 1/(w*x*y*z))^n, 0), 0), 0), 0);
for(n=0, M, print1(a(n), ", "));
