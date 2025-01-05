\\ E.g.f.: 2/(exp(-x) + sqrt(exp(-2*x) - 4*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace( 2/(exp(-x) + sqrt(exp(-2*x) - 4*x)) ))

