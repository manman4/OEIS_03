\\ Expansion of e.g.f. exp( -LambertW(-x * cos(x)) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp( -lambertw(-x * cos(x)) )))
