\\ Expansion of e.g.f. 1/(1 - arcsinh(2*x))^(1/2).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-asinh(2*x))^(1/2)))

\\ E.g.f.: 1/(1 - log(2*x + sqrt(4*x^2 + 1)))^(1/2).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-asinh(2*x))^(1/2) - 1/(1 - log(2*x + sqrt(4*x^2 + 1)))^(1/2)))