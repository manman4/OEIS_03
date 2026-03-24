\\ E.g.f.: (2/(1 + sqrt(1-12*x)))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace( (2/(1+sqrt(1-12*x)))^(1/3) ))

