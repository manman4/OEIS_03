\\ Expansion of e.g.f. (4 - 3 * exp(x))^(5/3).
my(N=25, x='x+O('x^N)); Vec(serlaplace((4 - 3 * exp(x))^(5/3)))


