\\ G.f.: Sum_{j>=0} [z^j] Product_{k>=1} (1 + z*q^(3*j*k)/(1-q^k)).
my(N=80, q='q+O('q^N)); Vec(sum(j=0, N, polcoef(prod(k=1, N, 1+z*q^(3*j*k)/(1-q^k)), j, z)))

