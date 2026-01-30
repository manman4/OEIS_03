M=30;

\\ Expansion of e.g.f. (1/x) * Series_Reversion( x/(1 + x*(exp(x^3) - 1)) ).
my(N=M, x='x+O('x^N)); Vec(serlaplace( serreverse( x/(1 + x*(exp(x^3) - 1)) )/x ))