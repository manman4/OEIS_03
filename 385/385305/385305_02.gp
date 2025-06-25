\\ Expansion of e.g.f. 1/(1 - 3 * sinh(x))^(1/3).
my(N=25, x='x+O('x^N)); Vec(serlaplace( 1/(1 - 3 * sinh(x))^(1/3) ))

