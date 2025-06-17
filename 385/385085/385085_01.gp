\\ E.g.f.: exp(-2/3 * LambertW(-3*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-2/3*lambertw(-3*x))))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A052752.
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-1/3*lambertw(-3*x))))