\\ Expansion of e.g.f. (exp(f(x)) - 1)^3 / 6, where f(x) = (exp(2*x) - 1)/2.
my(N=40, x='x+O('x^N), f(x) = (exp(2*x) - 1)/2); concat([0, 0, 0], Vec(serlaplace( (exp(f(x)) - 1)^3 / 6 )))
