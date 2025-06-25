\\ Expansion of e.g.f. 1/(1 - 2 * sin(x))^(1/2).
my(N=25, x='x+O('x^N)); Vec(serlaplace( 1/(1 - 2 * sin(x))^(1/2) ))

