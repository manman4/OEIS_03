\\ E.g.f.: ( (1/x) * Series_Reversion( x * (exp(-x) - x)^3 ) )^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace( ( serreverse(x*(exp(-x)-x)^3)/x )^(1/3) ))


