M=35;

\\ Diagonal of the rational function 1 / (1 - x^2 - y^2 - x^5*y^5).
a(n) = my(x='x+O('x^(n+1)), y='y+O('y^(n+1)) ); polcoef(polcoef(1 / (1 - x^2 - y^2 - x^5*y^5), n), n);
for(n=0, M, print1(a(n),", "));

\\ Diagonal of the rational function 1 / ((1-x^2*y)*(1-x^3*y^4) - y).
b(n) = my(x='x+O('x^(n+1)), y='y+O('y^(n+1)) ); polcoef(polcoef(1 / ((1-x^2*y)*(1-x^3*y^4) - y), n), n);
for(n=0, 100, print1(a(n)-b(n),", "));
