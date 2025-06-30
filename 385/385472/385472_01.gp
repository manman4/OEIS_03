\\ Expansion of e.g.f. 1/(1 - arctanh(2*x))^(1/2).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-atanh(2*x))^(1/2)))

\\ E.g.f.: 1/(1 - (1/2) * log((1+2*x)/(1-2*x)))^(1/2).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-atanh(2*x))^(1/2) - 1/(1 - (1/2) * log((1+2*x)/(1-2*x)))^(1/2)))