\\ Expansion of e.g.f. 2/(5 - 3*exp(2*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace( 2/(5 - 3*exp(2 * x)) ))


