\\ Expansion of e.g.f. (1/x) * Series_Reversion( x / (exp(-x) + x)^2 ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( 1/x * serreverse(x / (exp(-x) + x)^2) ))

