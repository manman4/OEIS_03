\\ Expansion of e.g.f. f(x)^2 * log(f(x)), where f(x) = 1/(1 - 5*x)^(1/5).
my(N=40, x='x+O('x^N), f(x) = 1/(1 - 5*x)^(1/5)); concat(0, Vec(serlaplace( f(x)^2 * log(f(x)) )))
