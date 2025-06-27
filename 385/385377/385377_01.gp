\\ Expansion of e.g.f. 1/(1 - 3 * arcsin(x))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-3*asin(x))^(1/3)))

