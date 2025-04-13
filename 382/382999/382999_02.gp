\\ G.f.: Sum_{k>=1} phi(k) * x^k / (1 + 2*x^k).
my(N=20, x='x+O('x^N)); Vec(sum(k=1, N, eulerphi(k)*x^k/(1+2*x^k)))

