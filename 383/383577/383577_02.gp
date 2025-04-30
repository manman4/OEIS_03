\\ Diagonal of the rational function 1 / ((1-x)*(1-y)*(1-z) - x^2*y^3*z^3).
a(n) = my(x='x+O('x^(n+1)), y='y+O('y^(n+1)), z='z+O('z^(n+1)) ); polcoef(polcoef(polcoef(1 / ((1-x)*(1-y)*(1-z) - x^2*y^3*z^3), n), n), n);
for(n=0, 35, print1(a(n),", "))         