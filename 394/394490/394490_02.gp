\\ Expansion of e.g.f. (1/x) * Series_Reversion( x/(1 + 5*x)^(3/5) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace(1/x * serreverse(x/(1 + 5*x)^(3/5))))

