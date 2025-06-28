\\ Expansion of e.g.f. exp( -LambertW(-arcsin(x)) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(-lambertw(-asin(x)))))

