\\ E.g.f.: 4*exp(x)/(1 + sqrt(1 - 4*x*exp(x)))^2.
my(x='x+O('x^20)); Vec(serlaplace(4*exp(x)/(1 + sqrt(1 - 4*x*exp(x)))^2))

