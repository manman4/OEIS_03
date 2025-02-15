\\ Expansion of e.g.f. exp( -LambertW(-x * cosh(x)) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp( -lambertw(-x * cosh(x)) )))
