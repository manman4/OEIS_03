\\ Expansion of ( (1/x) * Series_Reversion( x/((1+x) * C(x))^3 ) )^(1/3), where C(x) is the g.f. of A000108.
my(N=20, x='x+O('x^N)); Vec((serreverse(x/((1+x)*(1-sqrt(1-4*x))/(2*x))^3)/x)^(1/3))

