\\ Expansion of e.g.f. 1/(exp(-3*x) - 3*x)^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(exp(-3*x) - 3*x)^(1/3)))

