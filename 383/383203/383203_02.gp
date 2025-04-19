\\ Expansion of e.g.f. f(x) * exp(f(x)), where f(x) = (exp(2*x) - 1)/2.
my(N=40, x='x+O('x^N), f(x) = (exp(2*x) - 1)/2); concat(0, Vec(serlaplace( f(x) * exp(f(x)) )))
