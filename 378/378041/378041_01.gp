\\ E.g.f.: exp( -LambertW(-2*x/(1-x)^3)/2 )/(1-x).
my(N=30, x='x+O('x^N)); Vec(serlaplace( exp( -lambertw(-2*x/(1-x)^3)/2 )/(1-x) ))
