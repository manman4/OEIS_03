\\ Expansion of e.g.f. exp( -LambertW(-x*(1+x)^2) ) / (1+x).
my(N=30, x='x+O('x^N)); Vec(serlaplace( exp(-lambertw(-x*(1+x)^2))/(1+x) ))


