\\ E.g.f.: f(x) * log(f(x))^4 / 24, where f(x) = 1/(1 - 3*x)^(1/3).
my(N=40, x='x+O('x^N), f(x) = 1/(1 - 3*x)^(1/3)); concat([0, 0, 0, 0], Vec(serlaplace( f(x) * log(f(x))^4 / 24)))

