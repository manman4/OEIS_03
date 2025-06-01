\\ Expansion of e.g.f. 2/(7 - 5*exp(2*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace( 2/(7 - 5*exp(2 * x)) ))


