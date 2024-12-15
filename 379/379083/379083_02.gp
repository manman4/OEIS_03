\\ Expansion of (1/x) * Series_Reversion( x * (1/(1 + x) - x^3)^3 ).
my(N=30, x='x+O('x^N)); Vec(serreverse(x * (1/(1 + x) - x^3)^3 )/x)


