\\ https://oeis.org/wiki/Eulerian_polynomials#Three_identitie
f(n) = if(n==0, 1, sum(k=1, n, k!*stirling(n, k, 2)*(x-1)^(n-k)));
\\ G.f. of column k
A(k, n) = k*x*subst(f(k), x, k*x)/((1-x) * (1-k*x)^(k+1) + x*O(x^(n+1)));

T(n, k) = if(k==0, 1, polcoef(A(k, n), n));

for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")));  
