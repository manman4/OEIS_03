\\ Expansion of e.g.f. 1/(exp(-4*x) - 4*x*exp(-3*x))^(1/4).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(exp(-4*x) - 4*x*exp(-3*x))^(1/4)))

