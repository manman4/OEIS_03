M=15;

a(n) = polcoef(polcoef(polcoef((1 + x + y + z + 1/(x*y) + 1/(y*z) + 1/(z*x))^n, 0), 0), 0); 
for(n=0, M, print1(a(n), ", "));
