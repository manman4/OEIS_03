\\ Expansion of e.g.f. exp(x/2) / (1-4*x)^(1/8).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(x/2)/(1-4*x)^(1/8)))


