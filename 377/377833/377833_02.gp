\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * (1 - x)^3 * exp(-x) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace(serreverse( x * (1 - x)^3 * exp(-x) )/x))


