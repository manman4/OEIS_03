\\ E.g.f.: (1+x)^2 * exp( -LambertW(-x*(1+x)) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( (1+x)^2 * exp( -lambertw(-x*(1+x)) ) ))

