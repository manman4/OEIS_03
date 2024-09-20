\\ Expansion of (1/x) * Series_Reversion( x * (1-x-x^2)^4 ).
my(N=30, x='x+O('x^N)); Vec(serreverse( x * (1-x-x^2)^4 )/x)

\\ A365188
my(N=30, x='x+O('x^N)); Vec( (serreverse( x * (1-x-x^2)^4 )/x)^(1/4) )
