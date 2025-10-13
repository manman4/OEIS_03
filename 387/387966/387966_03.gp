\\ E.g.f.: (1/x) * Series_Reversion( x * (1-x) * exp(-x^2 / (1-x)^3) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( serreverse( x * (1-x) * exp(-x^2 / (1-x)^3) )/x ))


