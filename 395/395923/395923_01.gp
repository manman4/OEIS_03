default(parisize, 120000000);

\\ T(n,k) = [x^n] Sum_{l>=1} q^(k*l) * Product_{i=1..k} Product_{j=1..l} 1/(1 - q^(i+j-1))
T(n, k) = {my(q='q+O('q^(n+1))); polcoef(1+sum(l=1, n, q^(k*l)*prod(i=1, k, prod(j=1, l, 1/(1-q^(i+j-1))))), n)}; 

M=140;
cnt = 1;
for(n=1, M, for(k=1, n, write("b395923_1.txt", cnt, " ", T(n, k)); cnt++));