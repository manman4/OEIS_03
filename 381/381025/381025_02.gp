\\ Expansion of e.g.f. -log(1-x)^3 * exp(x) / (6 * (1-x)).
my(N=30, x='x+O('x^N)); concat([0, 0, 0], Vec(serlaplace( -log(1-x)^3 * exp(x) / (6 * (1-x)) )))
