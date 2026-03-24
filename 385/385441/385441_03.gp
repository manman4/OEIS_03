\\ E.g.f.: sqrt(2/(1 + sqrt(1-8*x))).
my(N=20, x='x+O('x^N)); Vec(serlaplace( sqrt(2/(1+sqrt(1-8*x))) ))

