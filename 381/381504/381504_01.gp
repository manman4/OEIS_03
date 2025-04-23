\\ Expansion of e.g.f. exp(-x/4) / (1-4*x)^(1/16).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(-x/4)/(1-4*x)^(1/16)))
