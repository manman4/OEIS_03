\\ E.g.f.: exp( -LambertW(-x^3 * (1+x)^2) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( exp(-lambertw(-x^3*(1+x)^2)) ))


