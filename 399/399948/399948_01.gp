\\ Expansion of e.g.f. 1/(1 + log(1 - x) * sinh(x)).
my(N=30, x='x+O('x^N)); Vec(serlaplace(1/(1+log(1-x)*sinh(x))))