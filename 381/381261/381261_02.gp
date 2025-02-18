\\ Expansion of e.g.f. exp( -LambertW(-2 * x * cos(x)) / 2 ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp( -lambertw(-2 * x * cos(x)) / 2 )))
