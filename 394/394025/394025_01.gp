\\ G.f.: Sum_{j>=1} q^(4*j+1) * Product_{k=j+1..3*j} (1+q^k).
my(N=80, q='q+O('q^N)); sum(j=1, N, q^(4*j+1) * prod(k=j+1, 3*j, (1+q^k)))
my(N=80, q='q+O('q^N)); Vec(sum(j=1, N, q^(4*j+1) * prod(k=j+1, 3*j, (1+q^k))))
