\\ E.g.f.: exp( arcsinh(x*exp(x)) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( exp( asinh(x*exp(x)) ) ))
