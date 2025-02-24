\\ E.g.f. ( (1/x) * Series_Reversion( x*(1 - sinh(x))^2 ) )^(1/2).
my(N=20, x='x+O('x^N)); Vec(serlaplace((serreverse( x*(1 - sinh(x))^2 )/x)^(1/2)))

