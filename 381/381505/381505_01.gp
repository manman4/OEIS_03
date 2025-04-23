\\ Expansion of e.g.f. exp(2*x/3) / (1-3*x)^(1/9).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(2*x/3)/(1-3*x)^(1/9)))


