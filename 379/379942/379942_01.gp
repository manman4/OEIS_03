\\ Expansion of e.g.f. 1/( exp(-x) - x )^3.
\\ E.g.f.: B(x)^3, where B(x) is the e.g.f. of A072597.

my(N=20, x='x+O('x^N)); Vec(serlaplace(1/( exp(-x) - x )^3))
