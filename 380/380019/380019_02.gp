\\ Expansion of e.g.f. 1/(1 - 4*x*exp(x))^(1/4).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1 - 4*x*exp(x))^(1/4)))
