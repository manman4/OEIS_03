\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * (1 - x)^2 * exp(-2*x) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( (1/x) * serreverse( x * (1 - x)^2 * exp(-2*x) )))

