\\ E.g.f.: (1+x) * exp( -LambertW(-x*(1+x)) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( (1+x) * exp( -lambertw(-x*(1+x))) ))

\\ E.g.f.: -LambertW(-x*(1+x))/x.
my(N=20, x='x+O('x^N)); Vec(serlaplace( -lambertw(-x*(1+x))/x ))


