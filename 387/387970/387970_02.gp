\\ Expansion of e.g.f. (1+x) * exp(x^2*(1+x)^3).
my(N=20, x='x+O('x^N)); Vec(serlaplace((1+x)*exp(x^2*(1+x)^3)))

