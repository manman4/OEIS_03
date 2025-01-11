\\ E.g.f.: x*exp(x) + sqrt(1 + x^2*exp(2*x)).
my(N=30, x='x+O('x^N)); Vec(serlaplace( x*exp(x) + sqrt(1 + x^2*exp(2*x)) ))

