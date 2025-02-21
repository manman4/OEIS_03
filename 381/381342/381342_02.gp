\\ Expansion of e.g.f. exp( x * cos(sqrt(2)*x) ).
my(N=20, x='x+O('x^N)); apply(round, Vec(serlaplace( exp(x * cos(sqrt(2)*x)) )))


