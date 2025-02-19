\\ Expansion of e.g.f. exp(sin(3*x) / 3).
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp(sin(3*x) / 3) ))

