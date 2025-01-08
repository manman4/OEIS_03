\\ Expansion of e.g.f. exp(-x)/(exp(-x) - x)^4.
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-x)/(exp(-x) - x)^4))
