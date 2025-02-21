\\ Expansion of e.g.f. 1/( 1 - sin(sqrt(2)*x) / sqrt(2) ).
my(N=20, x='x+O('x^N)); apply(round, Vec(serlaplace( 1/( 1 - sin(sqrt(2)*x) / sqrt(2) ) )))
