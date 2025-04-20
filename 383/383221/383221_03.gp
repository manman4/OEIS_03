\\ E.g.f.: f(x)^2 * log(f(x))^3 / 6, where f(x) = 1/(1 - 3*x)^(1/3).
my(N=40, x='x+O('x^N), f(x) = 1/(1 - 3*x)^(1/3)); concat([0, 0, 0], Vec(serlaplace( f(x)^2 * log(f(x))^3 / 6)))


