\\ Expansion of e.g.f. f(x)^2 * exp(f(x)) / 2, where f(x) = (exp(3*x) - 1)/3.
my(N=40, x='x+O('x^N), f(x) = (exp(3*x) - 1)/3); concat([0, 0], Vec(serlaplace( f(x)^2 * exp(f(x)) / 2 )))



