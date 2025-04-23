\\ Expansion of e.g.f. exp(-x/2) / (1-6*x)^(1/12).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(-x/2)/(1-6*x)^(1/12)))

