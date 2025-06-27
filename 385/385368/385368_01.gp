\\ Expansion of e.g.f. 1/(1 - 3 * arcsinh(x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-3*asinh(x))))

\\ E.g.f.: 1/(1 - 3 * log(x + sqrt(x^2 + 1))).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-3*asinh(x)) - 1/(1-3*log(x+sqrt(x^2+1)))))