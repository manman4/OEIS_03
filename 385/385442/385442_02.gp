\\ Expansion of e.g.f. (1/x) * Series_Reversion( x/(4*x + sqrt(16*x^2+1))^(1/4) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( serreverse(x/(4*x + sqrt(16*x^2+1))^(1/4))/x ))

\\ E.g.f.: (1/x) * Series_Reversion( x * exp(-arcsinh(4*x)/4) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( serreverse(x * exp(-asinh(4*x)/4))/x ))

\\ E.g.f.: ( (1/x) * Series_Reversion( x/(1 + 8*x)^(5/8) ) )^(1/5).
my(N=30, x='x+O('x^N)); Vec(serlaplace( ( serreverse(x/(1 + 8*x)^(5/8))/x )^(1/5) ))
