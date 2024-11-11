\\ E.g.f.: (1+x) * exp( -LambertW(-3*x*(1+x)^3)/3 ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( (1+x) * exp( -lambertw(-3*x*(1+x)^3)/3 )))

\\ E.g.f.: ( -LambertW(-3*x*(1+x)^3)/(3*x) )^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace( ( -lambertw(-3*x*(1+x)^3)/(3*x) )^(1/3) ))

