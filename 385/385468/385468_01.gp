\\ Expansion of e.g.f. 1/(1 - 2 * arctanh(x))^(1/2).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-2*atanh(x))^(1/2)))

\\ E.g.f.: 1/(1 - log((1+x)/(1-x)))^(1/2).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-2*atanh(x))^(1/2) - 1/(1 - log((1+x)/(1-x)))^(1/2)))