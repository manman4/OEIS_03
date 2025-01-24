\\ E.g.f.: A(x) = exp( -2*LambertW(-x * exp(x)) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-2*lambertw(-x*exp(x)))))

