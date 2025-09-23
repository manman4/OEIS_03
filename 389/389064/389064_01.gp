\\ Expansion of (1/x) * Series_Reversion( x / (1 + x^3 * (1 + x)^3) ).
my(N=30, x='x+O('x^N)); Vec(serreverse(x/(1 + x^3 * (1 + x)^3))/x)