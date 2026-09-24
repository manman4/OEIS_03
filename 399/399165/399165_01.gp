\\ Expansion of (1/x) * Series_Reversion( x * (1-4*x)^(3/2) ).
my(N=20, x='x+O('x^N)); Vec(serreverse(x*(1-4*x)^(3/2))/x)
