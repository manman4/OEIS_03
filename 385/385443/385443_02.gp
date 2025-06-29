\\ Expansion of e.g.f. (1/x) * Series_Reversion( x/(3*x + sqrt(9*x^2+1))^(1/3) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( serreverse(x/(3*x + sqrt(9*x^2+1))^(1/3))/x ))

\\ E.g.f.: (1/x) * Series_Reversion( x * exp(-arcsinh(3*x)/3) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( serreverse(x * exp(-asinh(3*x)/3))/x ))

\\ E.g.f.: ( (1/x) * Series_Reversion( x/(1 + 6*x)^(2/3) ) )^(1/4).
my(N=30, x='x+O('x^N)); Vec(serlaplace( ( serreverse(x/(1 + 6*x)^(2/3))/x )^(1/4) ))
