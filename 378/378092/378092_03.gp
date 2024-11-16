\\ E.g.f.: exp( -LambertW(-2*x/(1-x))/2 )/(1-x).
my(x='x+O('x^30)); Vec(serlaplace( exp( -lambertw(-2*x/(1-x))/2 )/(1-x) ))


