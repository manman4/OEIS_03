\\ G.f.: (1/x) * Series_Reversion( x*(1 - 4*x/(1-x))^(1/2) ).
my(N=30, x='x+O('x^N)); Vec(serreverse( x*(1 - 4*x/(1-x))^(1/2) )/x)
