\\ G.f.: Product_{k>0} ((1-x^(5*k-1)) * (1-x^(5*k-4)))^3 / ((1-x^k) * (1-x^(5*k))).
my(N=50, x='x+O('x^N)); Vec(prod(k=1, N, ((1-x^(5*k-1))*(1-x^(5*k-4)))^3 / ((1-x^k)*(1-x^(5*k)))))

