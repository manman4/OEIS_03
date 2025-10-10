\\ G.f.: (1/x) * Series_Reversion( x / (1 + x / (1 - x)^2)^3 ).
my(N=30, x='x+O('x^N)); Vec(serreverse(x / (1 + x / (1 - x)^2)^3)/x)

\\ G.f.: B(x)^3, where B(x) is the g.f. of A367280. 
my(N=30, x='x+O('x^N)); Vec( (serreverse(x / (1 + x / (1 - x)^2)^3)/x )^(1/3) )
