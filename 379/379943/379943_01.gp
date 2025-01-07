\\ Expansion of e.g.f. 1/( exp(-x) - x )^4.
\\ E.g.f.: B(x)^4, where B(x) is the e.g.f. of A072597.

my(N=20, x='x+O('x^N)); Vec(serlaplace(1/( exp(-x) - x )^4))
