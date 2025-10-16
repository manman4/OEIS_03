\\ E.g.f.: (1+x) * exp( -LambertW(-x^2 * (1+x)) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( (1+x) * exp(-lambertw(-x^2*(1+x))) ))


