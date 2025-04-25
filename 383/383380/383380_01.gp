\\ Expansion of e.g.f. exp(-2*x) / (1-x)^4.
\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A000255.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(-2*x)/(1-x)^4))



