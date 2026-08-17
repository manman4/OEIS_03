\\ Expansion of e.g.f. exp(sinh(x) + sinh(x)^2).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(sinh(x) + sinh(x)^2)))

