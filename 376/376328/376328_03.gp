\\ G.f.: (1/x) * Series_Reversion( x / (1+x+x^2)^5 ).
my(N=30, x='x+O('x^N)); Vec(serreverse( x / (1+x+x^2)^5 )/x)

\\ A365189
my(N=30, x='x+O('x^N)); Vec( (serreverse( x / (1+x+x^2)^5 )/x)^(1/5) )