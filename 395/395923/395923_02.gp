\\ G.f. of column k: Sum_{l>=1} q^(k*l) * Product_{i=1..k} Product_{j=1..l} 1/(1 - q^(i+j-1)).
T(n, k) = my(q='q+O('q^(n+1))); polcoef(sum(l=1, n, q^(k*l)*prod(i=1, k, prod(j=1, l, 1/(1-q^(i+j-1))))), n);
for(n=1, 12, for(k=1, n, print1(T(n, k),", ")));



