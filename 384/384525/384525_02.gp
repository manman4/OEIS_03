\\ Expansion of e.g.f. 5/(6 - exp(5*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace( 5/(6 - exp(5 * x)) ))


