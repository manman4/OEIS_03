\\ E.g.f.: (1+x) * exp( -LambertW(-2*x*(1+x)^2)/2 ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( (1+x) * exp( -lambertw(-2*x*(1+x)^2)/2 )))

\\ E.g.f.: ( -LambertW(-2*x*(1+x)^2)/(2*x) )^(1/2).
my(N=20, x='x+O('x^N)); Vec(serlaplace( ( -lambertw(-2*x*(1+x)^2)/(2*x) )^(1/2) ))


