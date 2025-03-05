\\ G.f.: exp( Sum_{k>=1} sigma_4(k^2) * x^k/k ).
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, sigma(k^2, 4)*x^k/k)))


