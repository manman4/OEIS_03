\\ Expansion of e.g.f. 1/(1 - x * exp(x))^4.
my(N=20, x='x+O('x^N)); Vec(serlaplace(1/(1 - x * exp(x))^4))



