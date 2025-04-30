M=11;

\\ G.f.: 1 / (1 - x - y - x^2*y^2).
a(n, k) = my(x='x+O('x^(n+1)), y='y+O('y^(k+1))); polcoef(polcoef(1/(1-x-y-x^2*y^2), n), k);
for(n=0, M, for(k=0, n, print1(a(k, n-k), ", ")));

