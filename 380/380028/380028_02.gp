\\ Expansion of e.g.f. sqrt(1 - 2*x*exp(x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace(sqrt(1 - 2*x*exp(x))))


