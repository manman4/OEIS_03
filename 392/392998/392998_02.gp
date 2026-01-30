M=20;

\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * (exp(-x) - x^3) ).
my(N=M, x='x+O('x^N)); Vec(serlaplace(serreverse( x * (exp(-x) - x^3) )/x ))
