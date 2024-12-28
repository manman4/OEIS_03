\\ E.g.f.: (1/x) * LambertW(x / (1 - 3*x)).
my(N=30, x='x+O('x^N)); Vec(serlaplace(lambertw( x / (1-3*x) )/x))

