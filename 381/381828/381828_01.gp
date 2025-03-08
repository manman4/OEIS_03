\\ Expansion of ( (1/x) * Series_Reversion( x * ((1-x) * (1-x+x^2))^2 ) )^(1/2).
my(N=20, x='x+O('x^N)); Vec((serreverse( x * ((1-x) * (1-x+x^2))^2 )/x)^(1/2))

