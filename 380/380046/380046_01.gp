\\ E.g.f.: exp( 2*arcsinh(x*exp(x)) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( exp( 2*asinh(x*exp(x)) ) ))

