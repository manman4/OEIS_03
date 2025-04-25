\\ Expansion of e.g.f. exp(-5*x) / (1-x)^5.
\\ E.g.f.: B(x)^5, where B(x) is the e.g.f. of A000166.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(-5*x)/(1-x)^5))

