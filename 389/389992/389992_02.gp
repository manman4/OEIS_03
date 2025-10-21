\\ Expansion of e.g.f. exp( x/(1-x)^2 ) / (1-x)^3.
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(x/(1-x)^2)/(1-x)^3))
