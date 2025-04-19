\\ Expansion of e.g.f. f(x)^3 * exp(f(x)) / 6, where f(x) = (exp(2*x) - 1)/2.
my(N=40, x='x+O('x^N), f(x) = (exp(2*x) - 1)/2); concat([0, 0, 0], Vec(serlaplace( f(x)^3 * exp(f(x)) / 6 )))
