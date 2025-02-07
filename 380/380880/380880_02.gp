\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * exp(-3*x*exp(x)) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( (1/x) * serreverse( x * exp(-3*x*exp(x)) )))


