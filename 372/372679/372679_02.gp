\\ G.f.: Sum_{k>=1} mu(15 * k) * x^k / (1 - x^k)^2, where mu() is the Moebius function (A008683).
my(N=80, x='x+O('x^N)); Vec(sum(k=1, N, moebius(15*k)*x^k/(1-x^k)^2))


