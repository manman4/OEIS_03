\\ Expansion of ( (1/x) * Series_Reversion( x/((1+x) * C(x))^4 ) )^(1/4), where C(x) is the g.f. of A000108.
my(N=20, x='x+O('x^N)); Vec((serreverse(x/((1+x)*(1-sqrt(1-4*x))/(2*x))^4)/x)^(1/4))
