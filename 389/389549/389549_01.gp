\\ Expansion of (1/x) * Series_Reversion( x * (1 - x)^2 / (1 + x^3 / (1 - x)^2) ).
my(N=20, x='x+O('x^N)); Vec(serreverse( x*(1-x)^2/(1+x^3/(1-x)^2) )/x)
