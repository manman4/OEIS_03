my(N=30, x='x+O('x^N)); Vec(serreverse( x*(1-x/(1-x))^4 )/x)

\\ G.f.: B(x)^4 where B(x) is the g.f. of A243667.
my(N=30, x='x+O('x^N)); Vec( ( serreverse( x*(1-x/(1-x))^4 )/x )^(1/4) )