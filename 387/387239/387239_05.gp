\\ E.g.f.: x^3/6 * exp(3*x) * BesselI(3, 2*x), with offset 3.
my(N=30, x='x+O('x^N)); Vec(serlaplace(x^3/6*exp(3*x)*besseli(3, 2*x)))
my(N=30, x='x+O('x^N)); serlaplace(x^3/6*exp(3*x)*besseli(3, 2*x))
