\\ Expansion of e.g.f. 1/(1 - 3 * arctanh(x))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-3*atanh(x))^(1/3)))

\\ E.g.f.: 1/(1 - (3/2) * log((1+x)/(1-x)))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-3*atanh(x))^(1/3) - 1/(1 - (3/2) * log((1+x)/(1-x)))^(1/3)))