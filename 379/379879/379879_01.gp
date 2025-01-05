\\ E.g.f.: 2*exp(-x)/(1 + sqrt(1 - 4*x*exp(-x))).
my(N=20, x='x+O('x^N)); Vec(serlaplace( 2*exp(-x)/(1 + sqrt(1 - 4*x*exp(-x))) ))

