\\ Expansion of e.g.f. exp(-x/3) / (1-3*x)^(1/9).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(-x/3)/(1-3*x)^(1/9)))


