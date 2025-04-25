\\ Expansion of e.g.f. exp(-4*x) / (1-x)^4.
\\ E.g.f.: B(x)^4, where B(x) is the e.g.f. of A000166.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(-4*x)/(1-x)^4))


