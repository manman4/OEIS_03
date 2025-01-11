\\ E.g.f.: sqrt( (1/x) * Series_Reversion(x/(1 + 2*x*exp(x))) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( sqrt(serreverse(x/(1 + 2*x*exp(x)))/x) ))



