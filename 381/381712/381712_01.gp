\\ G.f.: exp( Sum_{k>=1} sigma_2(k^3) * x^k/k ).
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, sigma(k^3, 2)*x^k/k)))

