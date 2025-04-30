M=36;

\\ Diagonal of the rational function 1 / (1 - x^2 - y^2 - x^7*y^7).
a(n) = my(x='x+O('x^(n+1)), y='y+O('y^(n+1)) ); polcoef(polcoef(1 / (1 - x^2 - y^2 - x^7*y^7), n), n);
for(n=0, M, print1(a(n),", "));

\\ Diagonal of the rational function 1 / ((1-x^2*y)*(1-x^5*y^6) - y).
b(n) = my(x='x+O('x^(n+1)), y='y+O('y^(n+1)) ); polcoef(polcoef(1 / ((1-x^2*y)*(1-x^5*y^6) - y), n), n);
for(n=0, 100, print1(a(n)-b(n),", "));
