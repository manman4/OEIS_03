\\ Expansion of e.g.f. 1/(1 - 2 * arcsinh(x))^(1/2).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-2*asinh(x))^(1/2)))

\\ E.g.f.: 1/(1 - 2 * log(x + sqrt(x^2 + 1)))^(1/2).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-2*asinh(x))^(1/2) - 1/(1-2*log(x+sqrt(x^2+1)))^(1/2)))