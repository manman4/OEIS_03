\\ Expansion of e.g.f. (1/x) * Series_Reversion( x*(1 + x*log(1-x^2))^3 ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( serreverse( x*(1 + x*log(1-x^2))^3 )/x ))


