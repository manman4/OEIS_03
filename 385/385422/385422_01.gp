\\ Expansion of e.g.f. 1/(1 - arcsin(3*x))^(1/3).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1-asin(3*x))^(1/3)))

