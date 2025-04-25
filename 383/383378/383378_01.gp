\\ Expansion of e.g.f. exp(-3*x) / (1-x)^4.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(-3*x)/(1-x)^4))

