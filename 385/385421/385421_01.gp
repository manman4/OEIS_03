\\ Expansion of e.g.f. 1/(1 - arcsin(2*x))^(1/2).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-asin(2*x))^(1/2)))