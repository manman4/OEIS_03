\\ Expansion of e.g.f. log(1 + x)/(1 - 2*x).
my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace( log(1 + x)/(1 - 2*x) )))
