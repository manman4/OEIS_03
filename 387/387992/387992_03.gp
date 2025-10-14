\\ E.g.f.: (1/x) * Series_Reversion( x * exp(-x^2 * (1+x)^2) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( serreverse( x * exp(-x^2 * (1+x)^2) )/x ))


