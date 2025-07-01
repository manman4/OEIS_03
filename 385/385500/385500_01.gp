\\ Expansion of e.g.f. exp( -LambertW(-arctanh(x)) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(-lambertw(-atanh(x)))))

