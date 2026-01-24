M=40;

\\ E.g.f.: (1/x) * Series_Reversion( x/(1 + x*(exp(x) - 1)^2) ).
my(N=M, x='x+O('x^N)); Vec(serlaplace( serreverse( x/(1 + x*(exp(x) - 1)^2) )/x ))