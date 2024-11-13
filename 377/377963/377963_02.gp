\\ Expansion of e.g.f. (1+x) * exp(x*(1+x)^2).
my(N=30, x='x+O('x^N)); Vec(serlaplace( (1+x) * exp(x*(1+x)^2) ))
