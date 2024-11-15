\\ E.g.f.: (1+x) * exp( -LambertW(-2*x*(1+x)^3)/2 ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( (1+x) * exp( -lambertw(-2*x*(1+x)^3)/2 ) ))

