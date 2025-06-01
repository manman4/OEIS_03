\\ Expansion of e.g.f. 3/(7 - 4*exp(3*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace( 3/(7 - 4*exp(3 * x)) ))


