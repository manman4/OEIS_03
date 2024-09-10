\\ Expansion of e.g.f. -LambertW(-x / (1 - 3*x)).
my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace( -lambertw(-x / (1 - 3*x)) )))

