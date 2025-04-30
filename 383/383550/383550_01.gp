M=10;

\\ G.f.: 1 / (1 - x^2 - y^2 - x^3*y^3).
a(n, k) = my(x='x+O('x^(n+1)), y='y+O('y^(k+1))); polcoef(polcoef(1/(1-x^2-y^2-x^3*y^3), n), k);
for(n=0, M, for(k=0, n, print1(a(k, n-k), ", ")));

\\ If n - k == 1 (mod 2), A(n,k) = 0.
for(n=0, 30, for(k=0, n, if((k - (n-k)) % 2 == 1, print1(a(k, n-k), ", "))));