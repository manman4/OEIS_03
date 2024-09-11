\\ Expansion of e.g.f. -LambertW(-2*x / (1 + x))/2.
my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace( -lambertw(-2*x / (1 + x))/2 )))

