\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * exp(-x*exp(3*x)) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( (1/x) * serreverse( x * exp(-x*exp(3*x)) )))
