\\ Expansion of e.g.f. (exp(-3*x) + 3*x)^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace( (exp(-3*x) + 3*x)^(1/3) ))

