\\ E.g.f.: (1/x) * Series_Reversion( x * (exp(-x) - x) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( serreverse( x * (exp(-x) - x) )/x ))
