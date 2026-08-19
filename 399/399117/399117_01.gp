\\ Expansion of e.g.f. 1/(cosh(x) - x*cosh(x)^4).
my(N=30, x='x+O('x^N)); Vec(serlaplace(1/(cosh(x)-x*cosh(x)^4)))