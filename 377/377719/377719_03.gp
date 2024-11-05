\\ E.g.f.: 4/(1 + sqrt(1 - 4*x*(exp(x) - 1)))^2.
my(N=20, x='x+O('x^N)); Vec(serlaplace( 4/(1 + sqrt(1 - 4*x*(exp(x) - 1)))^2 ))


