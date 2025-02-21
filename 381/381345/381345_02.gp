\\ Expansion of e.g.f. 1/( 1 - x * cos(sqrt(2)*x) ).
my(N=20, x='x+O('x^N)); apply(round, Vec(serlaplace( 1/( 1 - x * cos(sqrt(2)*x) ))))
