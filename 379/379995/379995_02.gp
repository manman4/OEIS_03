\\ Expansion of e.g.f. exp(-2*x)/(exp(-x) - x)^4.
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-2*x)/(exp(-x) - x)^4))
