\\ Expansion of e.g.f. -LambertW(-3*x / (1 - x))/3.
my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace( -lambertw(-3*x / (1 - x)) / 3 )))


