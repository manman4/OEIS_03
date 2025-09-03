\\ Expansion of e.g.f. exp(x/(1 - 9*x^3)^(1/3)).
my(N=25, x='x+O('x^N)); Vec(serlaplace(exp(x/(1 - 9*x^3)^(1/3))))
