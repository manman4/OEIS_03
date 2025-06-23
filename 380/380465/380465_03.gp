\\ G.f.: ( (1/x) * Series_Reversion(x/(1+25*x)^(7/5)) )^(1/7).
my(N=30, x='x+O('x^N)); Vec( ( serreverse(x/(1+25*x)^(7/5))/x )^(1/7) )

