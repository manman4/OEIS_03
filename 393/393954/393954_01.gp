\\ G.f.: B(x)^2, where B(x) is the g.f. of A192945.
my(N=20, x='x+O('x^N)); Vec(1+serreverse(x*(1-2*x-x^2)))

\\ Expansion of ( 1 + Series_Reversion( x*(1-2*x-x^2) ) )^2.
my(N=30, x='x+O('x^N)); Vec((1+serreverse(x*(1-2*x-x^2)))^2)