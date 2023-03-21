M=20;

a(n) = polcoef(polcoef(polcoef((1 + x*y + y*z + z*x + 1/(x*y*z))^n, 0), 0), 0); 
for(n=0, M, print1(a(n), ", "));

a(n) = polcoef(polcoef(polcoef((1 + x^2 + y^2 + z^2 + 1/(x*y*z))^n, 0), 0), 0); 
for(n=0, M, print1(a(n), ", "));
