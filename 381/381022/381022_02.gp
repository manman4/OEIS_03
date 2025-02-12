\\ Expansion of e.g.f. -log(1-x)^3 * exp(x) / 6.
my(N=30, x='x+O('x^N)); concat([0, 0, 0], Vec(serlaplace( -log(1-x)^3 * exp(x) / 6) ))



