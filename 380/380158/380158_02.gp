\\ Expansion of e.g.f. sqrt(exp(-2*x) + 2*x).
my(N=20, x='x+O('x^N)); Vec(serlaplace( sqrt(exp(-2*x) + 2*x) ))

