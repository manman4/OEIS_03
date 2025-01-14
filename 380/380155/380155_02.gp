\\ Expansion of e.g.f. 1/sqrt(1 - 2*x*exp(2*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace( 1/sqrt(1 - 2*x*exp(2*x)) ))

