\\ G.f.: ( (1/x) * Series_Reversion(x/(1+49*x)^(6/7)) )^(1/6).
my(N=30, x='x+O('x^N)); Vec( ( serreverse(x/(1+49*x)^(6/7))/x )^(1/6) )

