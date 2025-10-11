\\ Expansion of (1/x) * Series_Reversion( x * (1 - x / (1 - x)^3)^2 ).
my(N=30, x='x+O('x^N)); Vec(serreverse( x * (1 - x / (1 - x)^3)^2 )/x)

\\ G.f.: B(x)^2, where B(x) is the g.f. of A367241.
my(N=30, x='x+O('x^N)); Vec(( serreverse( x * (1 - x / (1 - x)^3)^2 )/x )^(1/2))
