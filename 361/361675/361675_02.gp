M=15;

a(n) = polcoef(polcoef(polcoef(polcoef((1 + x*y*z + w*y*z + w*x*z + w*x*y + 1/(w*x*y*z))^n, 0), 0), 0), 0); 
for(n=0, M, print1(a(n), ", "));

a(n) = polcoef(polcoef(polcoef(polcoef((1 + w^3 + x^3 + y^3 + z^3 + 1/(w*x*y*z))^n, 0), 0), 0), 0); 
for(n=0, M, print1(a(n), ", "));