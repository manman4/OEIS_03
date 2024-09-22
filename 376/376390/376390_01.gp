\\ Expansion of e.g.f. (1/x) * Series_Reversion( x*(2 - exp(x))^3 ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( serreverse( x*(2 - exp(x))^3 )/x ))


