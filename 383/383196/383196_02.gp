\\ Expansion of e.g.f. (1/(1 - 3*x)^(1/3) - 1)^3 / 6.
my(N=30, x='x+O('x^N)); concat([0, 0, 0], Vec(serlaplace( (1/(1 - 3*x)^(1/3) - 1)^3 / 6 )))
