\\ Expansion of e.g.f. exp( (1/(1-2*x)^(3/2) - 1)/3 ).
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp( (1/(1 - 2*x)^(3/2) - 1)/3)))

