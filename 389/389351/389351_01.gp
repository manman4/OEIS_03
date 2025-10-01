\\ Expansion of (1/x) * Series_Reversion( x * (1 - x^3 / (1 - x)^5) ).
my(N=40, x='x+O('x^N)); Vec(serreverse(x*(1 - x^3 / (1 - x)^5))/x)