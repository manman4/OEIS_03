\\ Expansion of e.g.f. exp(x * cosh(x)^4).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(x * cosh(x)^4)))