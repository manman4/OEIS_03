\\ Expansion of 1/sqrt(1 - 4*x*sqrt(1 + 4*x)).
my(N=30, x='x+O('x^N)); Vec( 1/(1 - 4*x*(1 + 4*x)^(1/2))^(1/2) )
