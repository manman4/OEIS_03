my(N=30, x='x+O('x^N)); Vec(serlaplace((1-x*sum(k=0, 1, binomial(1, k)*(-x)^k/(k+1)!)*exp(-x))/(1-x)))

\\ E.g.f.: (1 - x * (1-x/2) * exp(-x)) / (1-x).
my(N=30, x='x+O('x^N)); Vec(serlaplace((1-x*(1-x/2)*exp(-x))/(1-x)))

