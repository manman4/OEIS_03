\\ Expansion of e.g.f. ( (1/x) * Series_Reversion( x * exp(-x / (1 - x)^2) ) )^2.
my(N=20, x='x+O('x^N)); Vec(serlaplace( ( serreverse( x * exp(-x / (1 - x)^2) )/x )^2 ))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A364939.
my(N=20, x='x+O('x^N)); Vec(serlaplace( ( serreverse( x * exp(-x / (1 - x)^2) )/x )^1 ))
