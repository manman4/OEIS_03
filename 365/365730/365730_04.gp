\\ G.f.: (1/x) * Series_Reversion( x / (1+x^4+x^5) ).
my(N=50, x='x+O('x^N)); Vec(serreverse(x/(1+x^4+x^5))/x)
