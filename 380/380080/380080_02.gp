\\ Expansion of e.g.f. (1/x) * Series_Reversion( x / sqrt(1 + 2*x*exp(x)) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( serreverse( x / sqrt(1 + 2*x*exp(x)) )/x ))
