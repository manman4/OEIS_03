\\ Expansion of e.g.f. exp( 1 - 1/(1-5*x)^(1/5) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp(1 - 1/(1-5*x)^(1/5)) ))