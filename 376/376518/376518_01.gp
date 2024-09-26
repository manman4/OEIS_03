\\ E.g.f.: exp( -LambertW(-x^3 * (1+x)) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp( -lambertw(-x^3*(1+x)) ) ))



