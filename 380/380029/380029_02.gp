\\ Expansion of e.g.f. (1 - 3*x*exp(x))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace((1 - 3*x*exp(x))^(1/3)))



