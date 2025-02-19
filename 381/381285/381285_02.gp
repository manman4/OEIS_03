\\ Expansion of e.g.f. 1/(1 - sin(2*x) / 2).
my(N=20, x='x+O('x^N)); Vec(serlaplace( 1/(1 - sin(2*x) / 2) ))

