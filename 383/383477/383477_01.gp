M=10;

\\ G.f.: 1 / (1 - x - x^2 - x^3 - y).
a(n, k) = my(x='x+O('x^(n+1)), y='y+O('y^(k+1))); polcoef(polcoef(1/(1-x-x^2-x^3-y), n), k);
for(n=0, M, for(k=0, n, print1(a(k, n-k), ", ")));



