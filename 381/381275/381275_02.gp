\\ Expansion of e.g.f. exp(x * cos(2*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp(x * cos(2*x)) ))

