M=40;

\\ E.g.f.: (1/x) * Series_Reversion( x * (1 + sqrt(1 + 4*x^3*log(1-x^2)))/2 ).
my(N=M, x='x+O('x^N)); Vec(serlaplace( serreverse( x * (1 + sqrt(1 + 4*x^3*log(1-x^2)))/2 )/x ))