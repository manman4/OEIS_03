\\ E.g.f.: ( (1/x) * Series_Reversion(x*(1 - 3*x*exp(x))) )^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace( ( (1/x) * serreverse( x*(1 - 3*x*exp(x)) ) )^(1/3) ))






