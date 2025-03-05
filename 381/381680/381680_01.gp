\\ G.f.: exp( Sum_{k>=1} sigma_6(k^2)/sigma_3(k^2) * x^k/k ).
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, sigma(k^2, 6)/sigma(k^2, 3)*x^k/k)))