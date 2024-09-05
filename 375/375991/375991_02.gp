\\ Expansion of e.g.f. (3 - 2 * exp(x))^(3/2).
my(N=25, x='x+O('x^N)); Vec(serlaplace((3 - 2 * exp(x))^(3/2)))

