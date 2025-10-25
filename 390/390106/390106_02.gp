\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * exp(-3*x/(1-x)) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace(serreverse( x * exp(-3*x/(1-x)) )/x))

