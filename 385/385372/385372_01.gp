\\ Expansion of e.g.f. 1/(1 - 3 * arcsinh(x))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-3*asinh(x))^(1/3)))

\\ E.g.f.: 1/(1 - 3 * log(x + sqrt(x^2 + 1)))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-3*asinh(x))^(1/3) - 1/(1-3*log(x+sqrt(x^2+1)))^(1/3)))