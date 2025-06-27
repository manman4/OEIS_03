\\ Expansion of e.g.f. x + sqrt(x^2 + 1).
my(N=20, x='x+O('x^N)); Vec(serlaplace(x + sqrt(x^2 + 1)))

\\ E.g.f.: exp(arcsinh(x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace(x + sqrt(x^2 + 1) - exp(asinh(x))))

\\ E.g.f. A(x) satisfies A(x) = 1/A(-x).
my(N=20, x='x+O('x^N)); Vec(serlaplace(x + sqrt(x^2 + 1) - 1/(-x + sqrt((-x)^2 + 1))))