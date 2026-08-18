\\ Expansion of e.g.f. 1/(1 - x*cosh(x)^2).
my(N=30, x='x+O('x^N)); Vec(serlaplace(1/(1 - x*cosh(x)^2)))