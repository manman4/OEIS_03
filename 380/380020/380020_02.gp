\\ Expansion of e.g.f. 1/sqrt(exp(-2*x) - 2*x*exp(-x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/sqrt(exp(-2*x) - 2*x*exp(-x))))
