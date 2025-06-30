\\ Expansion of e.g.f. 1/(1 - arctanh(3*x))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-atanh(3*x))^(1/3)))

\\ E.g.f.: 1/(1 - (1/2) * log((1+3*x)/(1-3*x)))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-atanh(3*x))^(1/3) - 1/(1 - (1/2) * log((1+3*x)/(1-3*x)))^(1/3)))