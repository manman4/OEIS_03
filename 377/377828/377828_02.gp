\\ E.g.f.: (1+x)^3 * exp( -LambertW(-x*(1+x)^3) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( (1+x)^3 * exp( -lambertw(-x*(1+x)^3)) ))

\\ E.g.f.: -LambertW(-x*(1+x)^3)/x.
my(N=20, x='x+O('x^N)); Vec(serlaplace( -lambertw(-x*(1+x)^3)/x ))

