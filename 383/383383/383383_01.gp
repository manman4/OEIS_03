\\ Expansion of e.g.f. exp(-4*x) / (1-x)^5.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(-4*x)/(1-x)^5))


