\\ Expansion of e.g.f. 1/(1 - sin(x) - sin(x)^2).
my(N=30, x='x+O('x^N)); Vec(serlaplace(1/(1 - sin(x) - sin(x)^2)))

