\\ Expansion of e.g.f. 1/(1 - 3 * arctanh(x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-3*atanh(x))))

\\ E.g.f.: 1/(1 - (3/2) * log((1+x)/(1-x))).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-3*atanh(x)) - 1/(1 - (3/2) * log((1+x)/(1-x)))))
