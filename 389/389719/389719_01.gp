\\ G.f.: Sum_{k>=1} q^k * Product_{d|k} (1+q^d).
my(N=80, q='q+O('q^N)); sum(k=1, N, q^k*prod(d=1, k, if(k%d==0, 1+q^d, 1)))
my(N=80, q='q+O('q^N)); Vec(sum(k=1, N, q^k*prod(d=1, k, if(k%d==0, 1+q^d, 1))))
