\\ Expansion of e.g.f. ( (1/x) * Series_Reversion( x*(1 - x*(exp(x) - 1))^3 ) )^(2/3).
my(N=30, x='x+O('x^N)); Vec(serlaplace( ( (1/x) * serreverse( x*(1 - x*(exp(x) - 1))^3 ) )^(2/3) ))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A371273.
my(N=30, x='x+O('x^N)); Vec(serlaplace( ( (1/x) * serreverse( x*(1 - x*(exp(x) - 1))^3 ) )^(1/3) ))
