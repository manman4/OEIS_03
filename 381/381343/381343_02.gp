\\ Expansion of e.g.f. exp( sin(sqrt(2)*x) / sqrt(2) ).
my(N=20, x='x+O('x^N)); apply(round, Vec(serlaplace( exp( sin(sqrt(2)*x) / sqrt(2) ))))
