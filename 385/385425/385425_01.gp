\\ Expansion of e.g.f. exp( -LambertW(-arcsinh(x)) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(-lambertw(-asinh(x)))))

