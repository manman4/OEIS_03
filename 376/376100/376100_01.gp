\\ Expansion of e.g.f. -LambertW(-x / (1 - 2*x)).
my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace( -lambertw(-x / (1 - 2*x)) )))



