\\ G.f.: Sum_{k>=1} q^(k*(k+2)) * Product_{i=1..2} Product_{j=1..k} 1/(1 - q^(i+j-1)).
my(N=60, q='q+O('q^N)); sum(k=1, N, q^(k*(k+2))*prod(i=1, 2, prod(j=1, k, 1/(1-q^(i+j-1)))))
my(N=60, q='q+O('q^N)); concat([0, 0, 0], Vec(sum(k=1, N, q^(k*(k+2))*prod(i=1, 2, prod(j=1, k, 1/(1-q^(i+j-1)))))))

