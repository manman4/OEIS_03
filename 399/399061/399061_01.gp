\\ Expansion of e.g.f. exp(x * cosh(x)^2).
my(N=100, x='x+O('x^N)); Vec(serlaplace(exp(x*cosh(x)^2)))