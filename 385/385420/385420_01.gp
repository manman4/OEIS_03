\\ Expansion of e.g.f. 1/(1 - arcsinh(3*x))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-asinh(3*x))^(1/3)))

\\ E.g.f.: 1/(1 - log(3*x + sqrt(9*x^2 + 1)))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-asinh(3*x))^(1/3) - 1/(1 - log(3*x + sqrt(9*x^2 + 1)))^(1/3)))
