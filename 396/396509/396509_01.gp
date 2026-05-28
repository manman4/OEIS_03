\\ E.g.f.: exp(-1/2 * LambertW(-4*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-1/2*lambertw(-4*x))))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A052774.
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-1/4*lambertw(-4*x))))