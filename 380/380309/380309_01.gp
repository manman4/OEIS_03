\\ Expansion of e.g.f. exp( 1 - 1/(1-3*x)^(1/3) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp(1 - 1/(1-3*x)^(1/3)) ))
