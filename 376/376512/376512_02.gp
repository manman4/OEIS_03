\\ Expansion of e.g.f. exp(x^2 * (1 + x)).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(x^2 * (1 + x))))
