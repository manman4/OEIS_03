\\ E.g.f.: exp( -LambertW(-x^2 * (1+x)) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp( -lambertw(-x^2*(1+x)) ) ))


