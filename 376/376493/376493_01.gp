\\ E.g.f.: exp( -LambertW(-3*x^3 * (1+x))/3 ).
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-lambertw(-3*x^3*(1+x))/3)))
