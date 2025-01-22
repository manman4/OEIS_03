\\ Expansion of e.g.f. log(1 - x^2/2 * log(1 - x)).
my(N=20, x='x+O('x^N)); concat([0, 0, 0], Vec(serlaplace( log(1 - x^2/2 * log(1 - x)) )))
