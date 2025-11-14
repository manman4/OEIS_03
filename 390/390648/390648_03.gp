\\ E.g.f.: 2/(1 + sqrt(1 - 4*x^3*exp(x))).
my(N=30, x='x+O('x^N)); Vec(serlaplace( 2/(1 + sqrt(1 - 4*x^3*exp(x))) ))