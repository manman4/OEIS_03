\\ G.f.: Sum_{j>=1} q^j*(1+q^j) * Product_{k=j+1..2*j} (1-q^(3*k))/(1-q^k).
my(N=80, q='q+O('q^N)); sum(j=1, N, q^j*(1+q^j) * prod(k=j+1, 2*j, (1-q^(3*k))/(1-q^k)))
my(N=80, q='q+O('q^N)); Vec(sum(j=1, N, q^j*(1+q^j) * prod(k=j+1, 2*j, (1-q^(3*k))/(1-q^k))))
