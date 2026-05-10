\\ G.f.: Sum_{k>=1} q^(3*k) * Product_{i=1..3} Product_{j=1..k} 1/(1 - q^(i+j-1)).
my(N=50, q='q+O('q^N)); sum(k=1, N, q^(3*k)*prod(i=1, 3, prod(j=1, k, 1/(1-q^(i+j-1)))))
my(N=50, q='q+O('q^N)); concat([0, 0, 0], Vec(sum(k=1, N, q^(3*k)*prod(i=1, 3, prod(j=1, k, 1/(1-q^(i+j-1)))))))

