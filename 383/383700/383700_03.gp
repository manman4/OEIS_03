\\ E.g.f.: f(x) * log(f(x))^2 / 2, where f(x) = 1/(1 - 4*x)^(1/4).
my(N=40, x='x+O('x^N), f(x) = 1/(1 - 4*x)^(1/4)); concat([0, 0], Vec(serlaplace( f(x) * log(f(x))^2 / 2)))



