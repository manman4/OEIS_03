\\ Expansion of e.g.f. 1/(1 - x * cos(3*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace( 1/(1 - x * cos(3*x)) ))

