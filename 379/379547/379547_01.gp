\\ Expansion of (1/x) * Series_Reversion( x / ( (1+x)^2 * (1+2*x)^4 ) ).
my(N=30, x='x+O('x^N)); Vec(serreverse( x / ( (1+x)^2 * (1+2*x)^4 ) )/x)
