\\ Expansion of e.g.f. 1/(1 - 2 * arcsin(x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-2*asin(x))))