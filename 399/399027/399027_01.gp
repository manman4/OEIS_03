\\ E.g.f.: exp(B(x)), where B(x) is the e.g.f. of A066532.
my(N=30, x='x+O('x^N)); serlaplace(sinh(x) + sinh(x)^2)
my(N=30, x='x+O('x^N)); Vec(serlaplace(sinh(x) + sinh(x)^2))

\\ Expansion of e.g.f. exp(sinh(x) + sinh(x)^2).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(sinh(x) + sinh(x)^2)))

