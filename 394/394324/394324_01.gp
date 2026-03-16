\\ G.f.: Sum_{j>=1} q^(j^2)/(1-q^j) * Product_{k=1..j-1} (1+q^(j*k)/(1-q^k)).
my(N=80, q='q+O('q^N)); sum(j=1, N, q^(j^2)/(1-q^j)*prod(k=1, j-1, (1+q^(j*k)/(1-q^k))))
my(N=80, q='q+O('q^N)); Vec(sum(j=1, N, q^(j^2)/(1-q^j)*prod(k=1, j-1, (1+q^(j*k)/(1-q^k)))))