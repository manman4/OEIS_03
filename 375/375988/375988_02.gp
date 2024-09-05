\\ Expansion of e.g.f. (1 + 3 * log(1 - x))^(4/3).
my(N=25, x='x+O('x^N)); Vec(serlaplace((1 + 3 * log(1 - x))^(4/3)))

