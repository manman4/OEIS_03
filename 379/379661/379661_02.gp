\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * (exp(-2*x) - x) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( serreverse( x * (exp(-2*x) - x) )/x ))
