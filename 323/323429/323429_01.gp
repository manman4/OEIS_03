default(parisize, 120000000);

\\ G.f.: 1 + Sum_{k>=1, l>=1} q^(k*l) * Product_{i=1..k} Product_{j=1..l} 1/(1 - q^(i+j-1)).
v(n) = {my(N=50, q='q+O('q^(n+1))); 1+sum(k=1, n, sum(l=1, n\k, q^(k*l)*prod(i=1, k, prod(j=1, l, 1/(1-q^(i+j-1))))))}; 

M=100;
v=v(M);
for(n=0, M, write("b323429_1.txt", n, " ", polcoef(v, n)));