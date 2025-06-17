\\ E.g.f.: exp(-2/5 * LambertW(-5*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-2/5*lambertw(-5*x))))

\\ E.g.f.: B(x)^2, where B(x) is the e.g.f. of A052782.
my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(-1/5*lambertw(-5*x))))