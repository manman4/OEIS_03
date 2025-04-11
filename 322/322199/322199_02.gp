M=32;

\\ G.f.: P(x,y) = Product_{n>=1} 1/(1 - (x^n + y^n))
a322210(n, k) = my(x='x+O('x^(n+20)), y='y+O('y^(k+20))); polcoef(polcoef(1/prod(j=1, max(n, k)+10, 1 - (x^j + y^j)), n), k);
\\ a(n) = Sum_{k=0..n} 2^k * A322210(k,n-k).
a(n) = sum(k=0, n, 2^k * a322210(k, n-k));
for(n=0, M, print1(a(n), ", ")); 