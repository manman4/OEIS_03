\\ E.g.f.: (1/x) * Series_Reversion( x/(1 + x^3*(exp(x) - 1)) ).
my(N=25, x='x+O('x^N)); Vec(serlaplace( serreverse( x/(1 + x^3*(exp(x) - 1)) )/x ))