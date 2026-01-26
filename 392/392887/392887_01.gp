M=40;

\\ Expansion of e.g.f. (1/x) * Series_Reversion( x/(1 + log(1-x^2)^2/x) ).
my(N=M, x='x+O('x^N)); Vec(serlaplace( serreverse( x/(1 + log(1-x^2)^2/x) )/x ))