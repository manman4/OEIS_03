M=40;

\\ Expansion of e.g.f. (1/x) * Series_Reversion( x - (exp(x^2) - 1)^2 ).
my(N=M, x='x+O('x^N)); Vec(serlaplace( serreverse( x - (exp(x^2) - 1)^2 )/x ))