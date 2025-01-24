\\ E.g.f.: A(x) = exp( -3*LambertW(-x * exp(x)) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-3*lambertw(-x*exp(x)))))


