Bell_poly(n) = sum(k=0, n, stirling(n, k, 2)*x^k);

\\ E.g.f. of column k: B_k(x) * exp(x) / (1-x), where B_n(x) = Bell polynomials.
T(n, k) = my(x='x+O('x^(n+1))); n!*polcoef(Bell_poly(k) * exp(x) / (1-x), n);
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", "))); 
  