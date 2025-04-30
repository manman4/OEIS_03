\\ Diagonal of the rational function 1 / (1 - x - y - x^3*y^3).
a(n) = my(x='x+O('x^(n+1)), y='y+O('y^(n+1)) ); polcoef(polcoef(1 / (1 - x - y - x^3*y^3), n), n);
for(n=0, 45, print1(a(n),", "));
  
