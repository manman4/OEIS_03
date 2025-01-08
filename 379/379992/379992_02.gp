\\ Expansion of e.g.f. exp(-3*x)/(exp(-x) - x)^2.
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-3*x)/( exp(-x) - x )^2))

