\\ Expansion of e.g.f. log(1 - x)^2 * exp(3*x) / 2.
my(N=40, x='x+O('x^N)); concat(0, Vec(serlaplace( log(1 - x)^2 * exp(3*x) / 2)))
